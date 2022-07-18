
#define discovery_prefix "homeassistant"

//JSON Variables
DynamicJsonDocument device_doc(128);
DynamicJsonDocument attributes_doc(128);
StaticJsonDocument<512> discovery_doc;
char device_jsonString[128];
char attributes_jsonString[128];
char discovery_jsonString[512];
                                

void hassio_device() {
    // device":{"name":"AmbiSense","identifiers":"09B376","manufacturer":"albroteam","model":"Ambiv2","sw_version":"01.01"}
    // Purge old JSON data and Load new values
    device_doc.clear();
    device_doc["name"] = config.DeviceName;                             // Device friendly name
    device_doc["ids"] = ChipID;                                         // identifiers
    //device_doc["cns"] = String("['IP','" + WiFi.localIP().toString() + "']"); // connections
    device_doc["mf"] = BRANDName;                                       // manufacturer
    device_doc["mdl"] = MODELName;                                      // model
    device_doc["sw"] = SWVer;                                           // sw_version
}

void send_attributes() {
    serializeJson(attributes_doc, attributes_jsonString);               //Serialize JSON data to string
    //telnet_println("HASSIO Attributes: " + String(attributes_jsonString));
    mqtt_publish(mqtt_pathtele, "Attributes", String(attributes_jsonString));
}


void send_status_attributes() {
    attributes_doc.clear();
    attributes_doc["IP"] = WiFi.localIP().toString();                   // Device IP address
    attributes_doc["Location"] = config.Location;                       // Device Location
    attributes_doc["DEEPSleep"] = config.DEEPSLEEP;                     // DEEPSleep status
    attributes_doc["SLEEPTime"] = config.SLEEPTime;                     // DEEPSleep time
    attributes_doc["APMode"] = config.APMode;                           // APMode status
    attributes_doc["WEB"] = config.WEB;                                 // WEB status
    send_attributes();
}

void send_switch_attributes() {
    attributes_doc.clear();
    attributes_doc["auto_off_set"] = TIMER;                             // Device IP address
    attributes_doc["remaining_time"] = TIMER_Current;                   // Device Location
    send_attributes();
}

// HASSIO Configuration registration
void config_entity(String component = "sensor", String device_class = "Battery", String param = "") {
    //"sensor" have well defined classes: battery, humidity, illuminance, signal_strength, temperature, power, pressure, etc
    if(param == "") param = device_class;       // use the "device_class" as "param" value

    // Discovery Topic: // <discovery_prefix>/<component>/[<node_id>/]<object_id>/config
    String mqtt_pathhassio = String(discovery_prefix) + "/" + component + '/' + ChipID.c_str() + '-' + param + '/';

    
    // Purge old JSON data and Load new values
    discovery_doc.clear();
    discovery_doc["uniq_id"] = ChipID + '-' + param;
    discovery_doc["name"]    = ChipID + ' ' + param;
    discovery_doc["dev"]     = device_doc;

    if(component == "cover") {
        discovery_doc["device_class"]       = device_class;             // Device_class shutter
        discovery_doc["set_position_topic"] = mqtt_pathconf + param;    // set_position_topic
        discovery_doc["position_topic "]    = mqtt_pathtele + param;    // position_topic
    }
    
    if(component == "switch") {
        discovery_doc["stat_t"] = mqtt_pathtele + param;                // state_topic
        discovery_doc["cmd_t"]  = mqtt_pathconf + param;                // command_topic

        if(param == "Switch") discovery_doc["json_attr_t"] = mqtt_pathtele + "Attributes";
        //discovery_doc["stat_off"] = "0";
        //discovery_doc["stat_on"]  = "1";
        discovery_doc["pl_off"]  = "0";
        discovery_doc["pl_on"]   = "1";
    }


    if(component == "sensor") {
        if(device_class != "none") discovery_doc["device_class"] = device_class;
        discovery_doc["stat_t"] = mqtt_pathtele + param;                // state_topic
    
        if(param == "Status") discovery_doc["json_attr_t"] = mqtt_pathtele + "Attributes";

        if(device_class == "Battery" || device_class == "Humidity" || device_class == "illuminance") {
            discovery_doc["unit_of_meas"] = "%";
            discovery_doc["val_tpl"]      = "{{ value | int }}";
        }

        if(device_class == "signal_strength") {
            discovery_doc["unit_of_meas"] = "dBm";
            discovery_doc["val_tpl"]      = "{{ value | int }}";
        }

        if(device_class == "Temperature") {
            discovery_doc["unit_of_meas"] = "ºC";
            discovery_doc["val_tpl"]      = "{{value | float }}";
        }

        if(device_class == "Power") {
            discovery_doc["unit_of_meas"] = "W";
            discovery_doc["val_tpl"]      = "{{value | float }}";
        }

        if(device_class == "Voltage") {
            discovery_doc["unit_of_meas"] = "V";
            discovery_doc["val_tpl"]      = "{{value | float }}";
        }

        if(device_class == "Current") {
            discovery_doc["unit_of_meas"] = "A";
            discovery_doc["val_tpl"]      = "{{value | float }}";
        }

        if(device_class == "Energy") {
            discovery_doc["unit_of_meas"] = "Wh";
            discovery_doc["val_tpl"]      = "{{value | int }}";
        }
    }

    serializeJson(discovery_doc, discovery_jsonString);                     //Serialize JSON data to string
    //telnet_println("HASSIO Config: " + String(discovery_jsonString));
    mqtt_publish(mqtt_pathhassio, "config", String(discovery_jsonString), true);
           
    if(param == "Status") {delay(5); send_status_attributes();}
    if(param == "Switch") {delay(5); send_switch_attributes();}
}


void delete_entity(String component = "sensor", String device_class = "Battery", String param = "") {
    if(param == "") param = device_class;       // use the "device_class" as "param" value

    String mqtt_pathhassio = String(discovery_prefix) + "/" + component + '/' + ChipID.c_str() + '-' + param + '/';

    mqtt_publish(mqtt_pathhassio, "config", "", true);
}
