// MQTT Constants
#define MQTT_new_MAX_PACKET_SIZE 512                // Default: 256 bytes
#define MQTT_new_KEEPALIVE 60                       // Default: 15 seconds
#define MQTT_new_SOCKET_TIMEOUT 15                  // Default: 15 seconds
#include <PubSubClient.h>


// MQTT PATH Structure
// /location/deviceName/<Param>                     --> typically, used when publishing info/status
// /location/deviceName/configure                   --> typically, used when subscribing for actions

// EXAMPLEs
// TOPIC                        MESSAGE
// /outside/MailBox/Status      "Mains"                             --> Device status OK / LOWBat
// /room/Estore/RSSI            -89                                 --> WiFi signal strength value in dBm
// /kitchen/AmbiSense/BattLevel 33                                  --> Battery Level 0 - 100 %
// /kitchen/AmbSense/configure  {param:"DeepSleep", value:false}    --> Set DeepSleep feature to Enabled - Disabled
// /outside/MailBox/configure   {param:"LED", value:false}          --> Set True / False to turn LED ON/OFF


// MQTT Variables
static const String MQTT_state_Name[] = {
    "MQTT_CONNECTION_TIMEOUT",      // -4
    "MQTT_CONNECTION_LOST",         // -3
    "MQTT_CONNECT_FAILED",          // -2
    "MQTT_DISCONNECTED",            // -1
    "MQTT_CONNECTED",               // 0
    "MQTT_CONNECT_BAD_PROTOCOL",    // 1
    "MQTT_CONNECT_BAD_CLIENT_ID",   // 2
    "MQTT_CONNECT_UNAVAILABLE",     // 3
    "MQTT_CONNECT_BAD_CREDENTIALS", // 4
    "MQTT_CONNECT_UNAUTHORIZED"     // 5
};
int16_t MQTT_state = MQTT_DISCONNECTED;             // MQTT state
uint16_t MQTT_Retry = 125;                          // Timer to retry the MQTT connection
uint16_t MQTT_errors = 0;                           // MQTT errors Counter
uint32_t MQTT_LastTime = 0;                         // Last MQTT connection attempt time stamp


#if MQTT_Secure
    // Initialize MQTT Client
    PubSubClient MQTTclient(WiFiSec.getWiFiClient());
#else
    //WiFi Client initialization
    WiFiClient wifiClient;                          //  Use this for unsecure connection
    // Initialize MQTT Client
    PubSubClient MQTTclient(wifiClient);
#endif

// MQTT Functions //
String  MQTT_state_string(int mqttstate = MQTT_state){
   return MQTT_state_Name[map(mqttstate, -4, 5, 0 , 9)];
}

void getTime() {
    time_t now;
    struct tm timeinfo;
    configTime(-1 * 3600, 0, "pool.ntp.org", "time.nist.gov", "time.windows.com");
    do {
        now = time(nullptr);
        delay(100);
    } while (now < 8 * 3600 * 2 && millis() < 15000);
    gmtime_r(&now, &timeinfo);
    Serial.print("TIME Current time: ");
    Serial.print(asctime(&timeinfo));

}

String mqtt_pathtele() {
  return ChipID + "/";
}


String mqtt_pathconf() {
  return ChipID + "/";
}


void mqtt_publish(String pubpath, String pubtopic, String pubvalue, boolean toretain = false) {
    String topic = "";
    topic += pubpath; topic += pubtopic;
    // Send payload
    if (MQTT_state == MQTT_CONNECTED) {
        if (MQTTclient.publish(topic.c_str(), pubvalue.c_str(), toretain) == 1) telnet_println("MQTT published:  " + String(topic.c_str()) + " = " + String(pubvalue.c_str()));
        else {
            //flash_LED(2);
            telnet_println("");
            telnet_println("!!!!! MQTT message NOT published. !!!!!");
            telnet_println("");
        }
    }
}


void mqtt_subscribe(String subpath, String subtopic) {
    String topic = "";
    topic += subpath; topic += subtopic;
    if (MQTTclient.subscribe(topic.c_str())) telnet_println("subscribed to topic: " + topic);
    else telnet_println("Error on MQTT subscription!");
}


void mqtt_unsubscribe(String subpath, String subtopic) {
    String topic = "";
    topic += subpath; topic += subtopic;
    if( MQTTclient.unsubscribe(topic.c_str())) telnet_println("unsubscribed to topic: " + topic);
    else telnet_println("Error on MQTT unsubscription!");
}


void mqtt_connect() {
    if (WIFI_state != WL_CONNECTED) telnet_println( "MQTT ERROR! ==> WiFi NOT Connected!" );
    else {
        getTime();
        telnet_print("Connecting to MQTT Broker ... ");
        MQTTclient.setBufferSize(MQTT_new_MAX_PACKET_SIZE);
        MQTTclient.setKeepAlive(MQTT_new_KEEPALIVE);
        MQTTclient.setSocketTimeout(MQTT_new_SOCKET_TIMEOUT);
        MQTTclient.setServer(config.MQTT_Server, config.MQTT_Port);
        // Attempt to connect (clientID, username, password, willTopic, willQoS, willRetain, willMessage, cleanSession)
        if (MQTTclient.connect(ChipID.c_str(), config.MQTT_User, config.MQTT_Password, (mqtt_pathtele() + "Status").c_str(), 0, false, "UShut", true)) {
        //if (MQTTclient.connect(ChipID.c_str())) {
            MQTT_state = MQTT_CONNECTED;
            telnet_println( "[DONE]" );
            //mqtt_subscribe(mqtt_pathconf(), "+");
            mqtt_subscribe(mqtt_pathconf(), "configure");
        }
        else {
            MQTT_state = MQTTclient.state();
            telnet_println("MQTT ERROR! ==> " + MQTT_state_string(MQTT_state));
        };
    }
}


void mqtt_disconnect() {
    mqtt_unsubscribe(mqtt_pathconf(), "+");
    MQTTclient.disconnect();
    MQTT_state = MQTT_DISCONNECTED;
    telnet_println("Disconnected from MQTT Broker.");
}

void mqtt_restart() {
    mqtt_publish(mqtt_pathtele(), "Status", "Restarting");
    mqtt_disconnect();
    ESPRestart();
}
