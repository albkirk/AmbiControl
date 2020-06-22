// Expose Espressif SDK functionality
/*
extern "C" {
#include "user_interface.h"
  typedef void (*freedom_outside_cb_t)(uint8 status);
  int  wifi_regist
  er_send_pkt_freedom_cb(freedom_outside_cb_t cb);
  void wifi_unregister_send_pkt_freedom_cb(void);
  int  wifi_send_pkt_freedom(uint8 *buf, int len, bool sys_seq);
}
*/

//#include <ESP8266WiFi.h>
//#include <ESP8266mDNS.h>                        // Include the mDNS library
#include <time.h>
#include <WiFiSec.h>

// initialize WiFi Security
WiFiSec WiFiSec(CA_CERT_PROG, CLIENT_CERT_PROG, CLIENT_KEY_PROG);

// WiFi and Sniffer VARIABLEs
static const String WIFI_state_Name[] = {
	"WL_IDLE_STATUS",			// 0
    "WL_NO_SSID_AVAIL",			// 1
    "WL_SCAN_COMPLETED",		// 2
    "WL_CONNECTED",				// 3
    "WL_CONNECT_FAILED",		// 4
    "WL_CONNECTION_LOST",		// 5
    "WL_DISCONNECTED"			// 6
};
int WIFI_state = WL_DISCONNECTED;
int Last_WIFI_state = WL_NO_SHIELD;
unsigned int WIFI_Retry = 180;                  // Timer to retry the WiFi connection
unsigned long WIFI_LastTime = 0;                // Last WiFi connection attempt time stamp
int WIFI_errors = 0;                            // WiFi errors Counter


// Wi-Fi functions

String WIFI_state_string(int wifistate = WIFI_state) {
    return WIFI_state_Name[WIFI_state];
}

void wifi_disconnect() {
    WiFi.mode(WIFI_SHUTDOWN);
}

void wifi_connect() {
  //  Connect to WiFi acess point or start as Acess point
//  if ( WiFi.status() != WL_CONNECTED ) {
        if (config.APMode) {
            if (config.STAMode) WiFi.mode(WIFI_AP_STA);     // Setup ESP in AP+Station  mode
            else WiFi.mode(WIFI_AP);                        // Setup ESP in AP only mode
        }
        else {
            config.STAMode = true;                           // To force having any conectivity
            WiFi.mode(WIFI_STA);                             // Setup ESP in Station only mode
        }

        if (config.STAMode) {
            // Handle DHCP, IP address and hostname for the shield
            if (!config.DHCP) {
                WiFi.persistent(true);                   // required for fast WiFi registration
                // Static IP (No DHCP) may be handy for fast WiFi registration
                IPAddress StaticIP(config.IP[0], config.IP[1], config.IP[2], config.IP[3]);
                IPAddress Gateway(config.Gateway[0], config.Gateway[1], config.Gateway[2], config.Gateway[3]);
                IPAddress Subnet(config.Netmask[0], config.Netmask[1], config.Netmask[2], config.Netmask[3]);
                IPAddress DNS(config.Gateway[0], config.Gateway[1], config.Gateway[2], config.Gateway[3]);
                WiFi.config(StaticIP, Gateway, Subnet, DNS);
            };
            String host_name = String(config.Location + String("-") + config.DeviceName);
            WiFi.hostname(host_name.c_str());
            if( RTC_read() ) {
                // The RTC data was good, make a quick connection
                Serial.print("Connecting to WiFi network using RTD data... ");
                WiFi.begin( config.ssid, config.WiFiKey, rtcData.LastWiFiChannel, rtcData.bssid, true );
                WIFI_state = WiFi.waitForConnectResult(5000);
                if ( WIFI_state != WL_CONNECTED ) {
                    Serial.println(" ---ERROR!?!. Trying using config values. ");
                    WiFi.begin(config.ssid, config.WiFiKey);
                    WIFI_state = WiFi.waitForConnectResult(10000);
                };
            }
            else {
                // The RTC data was not valid, so make a regular connection
                Serial.print("NO RTD data. Using config values ... ");
                WiFi.begin(config.ssid, config.WiFiKey);
                WIFI_state = WiFi.waitForConnectResult(10000);
            }
            if ( WIFI_state == WL_CONNECTED ) {
                Serial.print("Connected to WiFi network! " + String(config.ssid) + " IP: "); Serial.println(WiFi.localIP());
                //rtcData.LastWiFiChannel = uint(wifi_get_channel);
                //if (!MDNS.begin(host_name)) {             // Start the mDNS responder for "host_name.local" domain
                //    Serial.println("Error setting up MDNS responder!");
                //}
                //else Serial.println("mDNS responder started");
            }
            else Serial.println( "WiFI ERROR! ==> " + WIFI_state_string(WIFI_state));
        }
        if (config.APMode) {
            WiFi.softAP(ESP_SSID.c_str());
            //WiFi.softAP(config.ssid);
            Serial.print("WiFi in AP mode, with IP: "); Serial.println(WiFi.softAPIP());
        }
//  }
//  else WIFI_state = WL_CONNECTED;
}


void wifi_setup() {
    WiFi.mode(WIFI_RESUME);
    wifi_connect();
}

void wifi_loop() {
    if ( WiFi.status() != WL_CONNECTED ) {
        if ( millis() - WIFI_LastTime > (WIFI_Retry * 1000)) {
            WIFI_errors ++;
            Serial.println( "in loop function WiFI ERROR! #: " + String(WIFI_errors) + "  ==> " + WIFI_state_string(WiFi.status()));
            WIFI_LastTime = millis();
            wifi_connect();
        }
    }
    else {
        if (WIFI_state != WL_CONNECTED) WIFI_state = WL_CONNECTED;
    }
    yield();
}
