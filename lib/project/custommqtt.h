// Function to insert customized MQTT Configuration actions

void  mqtt_custom(String command, String cmd_value) {

  if ( command == "Info" && bool(cmd_value.toInt())) {
    //status_report();
    if (BattPowered) mqtt_publish(mqtt_pathtele, "Battery", String(getBattLevel(),0));
    gps_update();
    if (GPS_Valid) mqtt_publish(mqtt_pathtele, "GPS_Valid", String(GPS_Sat));
    else mqtt_publish(mqtt_pathtele, "GPS_Valid", String(GPS_Valid));
    //mqtt_publish(mqtt_pathtele, "Switch", String(SWITCH));
    //mqtt_publish(mqtt_pathtele, "Timer", String(TIMER));
    mqtt_publish(mqtt_pathtele, "NTP_Sync", String(NTP_Sync));
    mqtt_publish(mqtt_pathtele, "DateTime", String(curDateTime()));
    //ambient_data();
    send_Telemetry();
  }
  if ( command == "Config" && bool(cmd_value.toInt())) {
    mqtt_publish(mqtt_pathtele, "OTA", String(config.OTA));
    mqtt_publish(mqtt_pathtele, "TELNET", String(config.TELNET));
    mqtt_publish(mqtt_pathtele, "WEB", String(config.WEB));
    mqtt_publish(mqtt_pathtele, "Interval", String(config.SLEEPTime));   
    mqtt_publish(mqtt_pathtele, "Temp_Corr", String(config.Temp_Corr));
    mqtt_publish(mqtt_pathtele, "LDO_Corr", String(config.LDO_Corr));
  }
  if ( command == "Interval" && bool(cmd_value.toInt())) { 
      config.SLEEPTime = byte(cmd_value.toInt());
      storage_write();
      //mqtt_publish(mqtt_pathtele, "Interval", String(config.SLEEPTime));
      send_status_attributes();
      mqtt_publish(mqtt_pathconf, "Interval", "", true);
      }
  if ( command == "Switch" && SWITCH_Last == bool(cmd_value.toInt())) mqtt_publish(mqtt_pathtele, "Switch", String(SWITCH));
  if ( command == "Telemetry" && bool(cmd_value.toInt())) send_Telemetry();
}


void custo_hassio(){
  config_entity("sensor","Temperature");
  config_entity("sensor","Humidity");
  //config_entity("sensor","illuminance", "Lux");
}
