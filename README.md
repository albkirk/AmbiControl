# **AmbiSense v2**  -  Ambient Sensor

This project builds a device to measure some ambient metrics, such: **Temperature** [ -40-80 °C ± 0.5 °C ] and **Humidity** [ 20-90% RH ± 2% RH ] and reports it using MQTT protocol.

##
## --Hardware--
This device is based on Espressif **ESP8266** MCU.

Temperature and Humidity are measured using the **AM2320** module.


As it is battery powered, the MCU is kept in sleepmode as mush possible and is only waked to measure the ambient and to report those values. Currently, a single 18650 Lithium Ion battey can last more than a month booting up on every 15 minutes. **Batt Charger** circuit included.

Simple **Push Button** is used to Start the MCU and execute several operations
1 Pulse   - <none>
2 Pulses  - <none>
3 Pulses  - Reboot
5 Pulses  - Turn off all administration channels (web, telnet, OTA, AP Mode)
10 Pulses - "Power Off" (actualy is deepsleep forever)
5+ Second Pressed - Factory Reset
Note: Push the button after once to wake up, wait to see the Led flash twice and act on the button.



##
## --Software--
Being based on my [GitHub albkirk ESP8266_KickStart](https://github.com/albkirk/ESP8266_KickStart), there's not much to add...

## --Dependencies--
You must install the following libraries (under arduino or Platformio):

 1. **ArduinoJson** by Benoit Blanchon 
 2. **PubSubClient** by Nick O'Leary 


## --Enclosure--
The suggested enclosure is shared in my Onshape CAD account: 
https://cad.onshape.com/documents/5abf8838a49b110243c2c383/w/94c9c8607583554f2e724389/e/0522379227c1ba5b54aa72e1


## Features
	1. Buffer Telemetry until connection is available -- Done using SPIFFS
	2. Add GPS to include geographical position
