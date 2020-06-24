// **** Project code definition here ...
#include <buttons.h>

#define FET_Control_Pin 0

// **** Project code functions here ...
void project_hw() {
 // Output GPIOs
    pinMode(FET_Control_Pin, OUTPUT);
    digitalWrite(FET_Control_Pin, false);    // if something goes wrong during setup, it allows to use the button as "RST button"
    buttons_setup();
    flash_LED(2);
    //pinMode(BUT_A, INPUT_PULLUP);
    //delay(300);
    //if (digitalRead(BUT_A) == false) {
    //  delay(5000);
    //  if (digitalRead(BUT_A) == false) {storage_reset(); ESPRestart();}
    //}


 // Input GPIOs

}


void project_setup() {
      //digitalWrite(FET_Control_Pin, false);   // Normal button operation
      //flash_LED(3);
      //buttons_setup();
  // Start Ambient devices
      ambient_setup();
      TIMER = 0;                             // TIMER value (Recommended 15 minutes) to get Ambient data.
      ambient_data();
}

void project_loop() {
  // Ambient handing
      if (config.SLEEPTime >0) if ((millis() - 3500) % (config.SLEEPTime * 60000) < 5) ambient_data();      // TIMER bigger than zero on div or dog bites!!


        if (A_COUNT == 1 && (millis() - last_A > 5000)) {
          mqtt_publish(mqtt_pathtele(), "Button", "Holded");
          flash_LED(20);
          mqtt_publish(mqtt_pathtele(), "Status", "Reseting");
          mqtt_disconnect();
          storage_reset();
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
