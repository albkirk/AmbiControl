// Function to insert customized MQTT Configuration actions

void  mqtt_custom(String reqtopic, String reqparam, StaticJsonDocument<200> data) {
  if ( reqparam == "Info" && bool(data["value"])) {
    //status_report();
    if (BattPowered) mqtt_publish(mqtt_pathtele(), "BattLevel", String(getVoltage()));
    mqtt_publish(mqtt_pathtele(), "Interval", String(config.DEEPSLEEP));
    gps_update();
    if (GPS_Valid) mqtt_publish(mqtt_pathtele(), "GPS_Valid", String(GPS_Sat));
    else mqtt_publish(mqtt_pathtele(), "GPS_Valid", String(GPS_Valid));
    mqtt_publish(mqtt_pathtele(), "NTP_Sync", String(NTP_Sync));
    mqtt_publish(mqtt_pathtele(), "DateTime", String(curDateTime()));
  }
  if ( reqparam == "Config" && bool(data["value"])) {
    mqtt_publish(mqtt_pathtele(), "OTA", String(config.OTA));
    mqtt_publish(mqtt_pathtele(), "TELNET", String(config.TELNET));
    mqtt_publish(mqtt_pathtele(), "WEB", String(config.WEB));
    mqtt_publish(mqtt_pathtele(), "GPS", String(config.GPS_HW));
    mqtt_publish(mqtt_pathtele(), "Interval", String(config.SLEEPTime));   
  }
  if ( reqparam == "Interval" && bool(data["value"])) { config.SLEEPTime = data["value"];storage_write(); mqtt_publish(mqtt_pathtele(), "Interval", String(config.SLEEPTime));}
  if ( reqparam == "Switch" && SWITCH_Last == bool(data["value"])) mqtt_publish(mqtt_pathtele(), "Switch", String(SWITCH));
  if ( reqparam == "Telemetry" && bool(data["value"])) send_Telemetry();
}
