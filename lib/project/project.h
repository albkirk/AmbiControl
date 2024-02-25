// WRITE HERE all the Project's logic / Flow.
#include <telemetry.h>


// **** Project code functions here ...


void project_setup() {
    rtcData.ByteValue = 0;
    rtcData.FloatValue = Humidity;
    //ambient_send_data();              // or ambient_data() to run ambient_get_data() again
    send_Telemetry();                   // Used for CGE-Vodafone project
}

void project_loop() {
  // Ambient handing
    if (config.SLEEPTime >0) if ((millis() + 3500) % (config.SLEEPTime * 60000) < 3) ambient_data(); //send_Telemetry();
//    if (config.HW_Module) if ((millis() - 400) % (GPS_Update * 1000) < 5) {   //  Update GPS data
//        gps_update();
//        print_gps_data();
//    }


        if (A_COUNT >= 1 && A_STATUS && (millis() - Last_A > 5000)) {
            mqtt_publish(mqtt_pathtele, "Button", "Holded");
            flash_LED(20);
            global_reset();
            A_COUNT = 0;
        }


        if (A_COUNT >= 1 && !A_STATUS && (millis() - Last_A > Butt_Interval)) {
            mqtt_publish(mqtt_pathtele, "Button", String(A_COUNT));
            flash_LED(A_COUNT);
            if (A_COUNT == 3) global_restart();

            if (A_COUNT == 5) {
                config.TELNET = false;
                config.OTA = false;
                config.WEB = false;
                config.APMode = false;
                config.LED = false;
                config.DEEPSLEEP = true;
                storage_write();
                web_setup();                // needed to clean the ON extender time.
                telnet_setup();
            }        
            if (A_COUNT == 6) {          
                config.WEB = true;
                config.APMode = true;
                config.LED = true;
                wifi_connect();
                web_setup();
            }
            if (A_COUNT == 7){
                config.TELNET = !config.TELNET;
                telnet_setup();
            }
            if (A_COUNT == 8) config.OTA = !config.OTA;
            if (A_COUNT == 9) config.Remote_Allow = !config.Remote_Allow;
            if (A_COUNT == 10) {
                mqtt_publish(mqtt_pathtele, "Status", "Shutdown");
                mqtt_disconnect();
                telnet_println("Going to sleep forever.");
                GoingToSleep(0, curUnixTime());
            }
            if (A_COUNT == 15) global_reset();      //FormatConfig();
            A_COUNT = 0;
        }


}
