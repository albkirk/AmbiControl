// Declare and define here all sensors and actuators used in this project
// **** Project code definition here ...
#include <buttons.h>
#include <ambient.h>
#include <mygps.h>

#define FET_Control_Pin 0


void peripherals_setup() {
 // Output GPIOs
    pinMode(FET_Control_Pin, OUTPUT);
    //digitalWrite(FET_Control_Pin, true);    // if something goes wrong during setup, it allows to use the button as "RST button"
    digitalWrite(FET_Control_Pin, false);     // Set button to Normal "Action" operation
 
    buttons_setup();
    // Start Ambient devices
    ambient_setup();


 // Input GPIOs

   // Avoid full setup cycle if Humidity didn't changed since last read.
   ambient_get_data();
   //telnet_println("Button is " + String(!bool(digitalRead(BUT_A))));
   if (ESPWakeUpReason() == "Deep-Sleep Wake" && RTC_read() && bool(digitalRead(BUT_A))) {  // If true, I can also assume rtc.ByteValue >= 0
      HumVelocity = (Humidity-rtcData.FloatValue)/SLEEPTime;
      telnet_println("HumVelocity: " + String(HumVelocity, 2));
      if ( (rtcData.FloatValue - Humidity) <=3 && (rtcData.FloatValue - Humidity) >=-3 && rtcData.ByteValue <7 ) {              // No change on humidity, so back to deep-sleep
        rtcData.ByteValue ++;
        rtcData.FloatValue = Humidity;
        telnet_println("No big change on Humidity, back to deep-sleep for " + String(config.SLEEPTime) + " min, or until next event ... zzZz :) ");
        telnet_println("Total time ON: " + String(millis()) + " msec");
        GoingToSleep(config.SLEEPTime, rtcData.lastUTCTime);
      };
   };
   gps_setup();
   flash_LED(2);

}

