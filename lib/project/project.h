// **** Project code definition here ...
#include <buttons.h>
//#include <ambient.h>
//#include <mygps.h>

#define FET_Control_Pin 0

//JSON Variables
char telemetry_jsonString[256];
DynamicJsonDocument telemetry_doc(256);


// **** Project code functions here ...
void print_gps_data () {
        if (GPS_Valid) {
            telnet_print("Sats: " + String(GPS_Sat) + "\t");
            telnet_print("Lat: " + String(GPS_Lat, 4) + "\t");
            telnet_print("Lng: " + String(GPS_Lng, 4) + "\t");
            telnet_print("Alt: " + String(GPS_Alt, 1) + "\t");
            telnet_print("Course: " + String(GPS_Course, 1) + "\t");
            telnet_print("Speed: " + String(GPS_Speed, 1) + "\t");
            telnet_println("");
        }
        else telnet_println("GPS Not FIX");
}


void send_Telemetry() {
    char fbat[3];        // long enough to hold complete floating string
    ambient_get_data();
    // Purge old JSON data and Load new values
    telemetry_doc.clear();
    telemetry_doc["Timestamp"] = curUnixTime();
    telemetry_doc["BatLevel"] = String(dtostrf(getVoltage(),3,0,fbat)).toFloat();
    telemetry_doc["RSSI"] = getRSSI();
    telemetry_doc["Temperature"] = Temperature;
    telemetry_doc["Humidity"] = Humidity;
    if (config.GPS_HW){
        gps_update();
        if (GPS_Valid){
            telemetry_doc["Lat"] = String(GPS_Lat, 6).toFloat();
            telemetry_doc["Lng"] = String(GPS_Lng, 6).toFloat();
        }
    }

    serializeJson(telemetry_doc, telemetry_jsonString);             //Serialize JSON data to string
    //telnet_println("Telemetry: " + String(telemetry_jsonString));
    mqtt_publish(mqtt_pathtele(), "Telemetry", String(telemetry_jsonString));
}


void project_hw() {
 // Output GPIOs
    pinMode(FET_Control_Pin, OUTPUT);
    //digitalWrite(FET_Control_Pin, true);    // if something goes wrong during setup, it allows to use the button as "RST button"
    digitalWrite(FET_Control_Pin, false);   // Set button to Normal "Action" operation
    buttons_setup();
    flash_LED(2);

 // Input GPIOs

}


void project_setup() {
      //digitalWrite(FET_Control_Pin, false);   // Normal button operation
      //buttons_setup();
      //flash_LED(2);
  // Start Ambient devices
      ambient_setup();
      //TIMER = 0;                             // TIMER value (Recommended 15 minutes) to get Ambient data.
      //ambient_data();
      gps_setup();
      send_Telemetry();
}

void project_loop() {
  // Ambient handing
    if (config.SLEEPTime >0) if ((millis() + 3500) % (config.SLEEPTime * 60000) < 5) send_Telemetry();  // TIMER bigger than zero on div or dog bites!!
//    if (config.GPS_HW) if ((millis() - 400) % (GPS_Update * 1000) < 5) {                                //  Update GPS data
//        gps_update();
//        print_gps_data();
//    }


        if (A_COUNT == 1 && (millis() - last_A > 5000)) {
          mqtt_publish(mqtt_pathtele(), "Button", "Holded");
          flash_LED(20);
          mqtt_publish(mqtt_pathtele(), "Status", "Reseting");
          mqtt_disconnect();
          storage_reset();
          RTC_reset();
          ESPRestart();
          A_COUNT = 0;
        }


        if (A_COUNT >= 1 && !A_STATUS && (millis() - last_A > Butt_Interval)) {
            mqtt_publish(mqtt_pathtele(), "Button", String(A_COUNT));
            flash_LED(A_COUNT);
            if (A_COUNT == 3) {
                mqtt_publish(mqtt_pathtele(), "Status", "Rebooting");
                mqtt_disconnect();
                ESPRestart();
            }
            if (A_COUNT == 5) {
                config.TELNET = false;
                config.OTA = false;
                config.WEB = false;
                config.APMode = false;
                config.LED = false;
                web_setup();                // needed to clean the ON extender time. 
                storage_write();
            }        
            if (A_COUNT == 6) {          
                config.WEB = !config.WEB;
                config.APMode = !config.APMode;
                config.LED = !config.LED;
            }
            if (A_COUNT == 7) config.TELNET = !config.TELNET;
            if (A_COUNT == 8) config.OTA = !config.OTA;
            if (A_COUNT == 9) config.Remote_Allow = !config.Remote_Allow;
            if (A_COUNT == 10) {
                mqtt_publish(mqtt_pathtele(), "Status", "Shutdown");
                mqtt_disconnect();
                telnet_println("Going to sleep forever.");
                GoingToSleep(0, curUnixTime());
            }
            A_COUNT = 0;
        }


}
