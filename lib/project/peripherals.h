// Declare and define here all sensors and actuators used in this project
// **** Periphericals definition here ...
#ifdef ModelVariant
   #define Batt_Lux_Switch
   #define FET_Control_Pin 4                 // Base of Transistor connected to PIN GPIO4, acting as Switch
#else
   #define FET_Control_Pin 0                 // MOSFET controlled by PIN GPIO0, acting as Wake trigger mask
#endif


// **** Libraries to include here ...
#include <buttons.h>
//#include <touch.h>
#include <ambient.h>
#include <mygps.h>

// **** Peripherals and critical functions here ...
void custom_ambient_get_data() {
#ifdef ModelVariant
    digitalWrite(FET_Control_Pin, HIGH);   // Set ADC Switch to HIGH to sample Lux (BC547 Saturated)
    delay(100);
    telnet_println("ADC Switch in Lux position");
#endif
   ambient_get_data();
#ifdef ModelVariant
    digitalWrite(FET_Control_Pin, LOW);   // Set ADC Switch to LOW to sample Battery (BC547 Cut)
    telnet_println("ADC Switch in VOLTAGE position");
#endif
}

void on_wakeup() {
   // Avoid full setup cycle if Humidity didn't changed since last read.
   custom_ambient_get_data();
   //telnet_println("Button is " + String(!bool(digitalRead(BUT_A))));
   if (ESPResetReason() == "Deep-Sleep Wake" && RTC_read() && bool(digitalRead(BUT_A))) {  // If true, I can also assume rtc.ByteValue >= 0
      HumVelocity = (Humidity-rtcData.FloatValue)/config.SLEEPTime;
      telnet_println("HumVelocity: " + String(HumVelocity, 2));
      if ( Humidity < float(98.0) && (rtcData.FloatValue - Humidity) <= 3 && (rtcData.FloatValue - Humidity) >= -3 && rtcData.ByteValue < 7 ) {              // No change on humidity, so back to deep-sleep
        rtcData.ByteValue ++;
        rtcData.FloatValue = Humidity;
        telnet_println("No big change on Humidity, back to deep-sleep for " + String(config.SLEEPTime) + " min, or until next event ... zzZz :) ");
        telnet_println("Total time ON: " + String(millis()) + " msec");
        esp_deepsleep(config.SLEEPTime * 60UL, rtcData.lastUTCTime);
      };
   };

}

// One of the 1st functions to be called on main.cpp in order to initialize critical components,
// such as: GPIO config, sensors, actuators, wake-up validation...  
void peripherals_setup() {
// Output GPIOs
    pinMode(FET_Control_Pin, OUTPUT);
    //digitalWrite(FET_Control_Pin, true);    // if something goes wrong during setup, it allows to use the button as "RST button"
    digitalWrite(FET_Control_Pin, false);     // Set button to Normal "Action" operation

// Input GPIOs

// Setup functions
   // Start buttons
      buttons_setup();
      //touch_setup();    
  // Start Ambient devices
      ambient_setup();

// Wake-up validation
    on_wakeup();

    gps_setup();
    flash_LED(2);
}

