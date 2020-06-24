#include "Adafruit_Sensor.h"
#include <DHT.h>
#include <Wire.h>
#include <AM2320.h>

// Initialize DHT/AM sensor.
#define DHT_11 1
#define DHT_22 2
#define AM_2320 3

#if DHTTYPE == 1
    #define DHTMODEL DHT11                 // using the DHT11 Model
#else
    #define DHTMODEL DHT22                 // using the DHT22 Model either for DHTTYPE == 2 or 3
#endif
DHT dht_val(DHTPIN, DHTMODEL);
AM2320 am_val(&Wire);

//NTC temperature
#define Rs 47000                            // 47KOhm Voltage Dividor Resistor

float Temperature = 0.0;                    // Variable
float Humidity = 0.0;                       // Variable
float Lux = 0.0;                            // Variable
float Tempe_MAX = -100.0;                   // Variable
float Tempe_MIN = 100.0;                    // Variable

//JSON Variables
char ambient_jsonString[256];
DynamicJsonDocument ambient_doc(256);

void I2C_scan() {
    byte error, address;
    int nDevices;

    Serial.println("Scanning...");

    nDevices = 0;
    for(address = 1; address < 127; address++ ) {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address<16)
            Serial.print("0");
            Serial.print(address,HEX);
            Serial.println(" !");

            nDevices++;
        }
        else if (error==4) {
            Serial.print("Unknow error at address 0x");
            if (address<16)
            Serial.print("0");
            Serial.println(address,HEX);
        }
    }

    if (nDevices == 0) Serial.println("No I2C devices found\n");
    else Serial.println("done\n");
}


float getNTCThermister() {
  // Return temperature as Celsius
  int val = 0;
  for(int i = 0; i < Number_of_measures; i++) {         // ADC value is read N times
      val += analogRead(A0);
      delay(10);
  }
  val = val / Number_of_measures;

  double V_NTC = (double)val / 1024;
  double R_NTC = (Rs * V_NTC) / (Vcc - V_NTC);
  R_NTC = log(R_NTC);
  double Tmp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * R_NTC * R_NTC ))* R_NTC );
  float Temp = (float)Tmp - 273.15 + config.Temp_Corr;
  return Temp;
}

float getTemperature() {
    // Return temperature as Celsius (DHT22 range -40 to +125 degrees Celsius) or -100 if error
    if (DHTPIN>=0 || SDAPIN>=0) {
        float t;
        int n = 0;
        int chk = 0;

        while (n < 10) {
            if (DHTTYPE == DHT_11 || DHTTYPE == DHT_22) {
                //Serial.println("CHK value: " + String(chk));
                t = dht_val.readTemperature();
                chk = int(isnan(t));
            }
            if (DHTTYPE == AM_2320) {
                chk = int(am_val.Read());
                t = am_val.temperature;
            }
            //Serial.print("Temperature: " + String(t));
            // Check if any reads failed and exit.
            if (chk != 0) {
                Serial.println("Failed to read temperature from DHT sensor!");
                delay(1000);
                //t = NULL;
                n ++;
            }
            else {
                //Serial.print("Temperature: " + String(t));
                return  t + config.Temp_Corr;
            }
        };
    }
    else {
        return -100; //getNTCThermister();
    }
    return -100;
}


float getHumidity() {
    // Read Humidity as Percentage (0-100%) or -1 if error

  float h;
  int n = 0;
  int chk = 0;

  while (n < 10 ) {
    if (DHTTYPE == DHT_11 || DHTTYPE == DHT_22) {
        h = dht_val.readHumidity();
        chk = int(isnan(h));
    }
    if (DHTTYPE == AM_2320) {
        chk = int(am_val.Read());
        h = am_val.humidity;
    }
    //Serial.print("Humidity: " + String(h));
    // Check if any reads failed and exit.
    if (chk != 0) {
      Serial.println("Failed to read humidity from DHT sensor!");
      delay(1000);
      //h = NULL;
      n ++;
    }
    else {
      //Serial.print("Humidity: " + String(h));
      return h;
    }
  }
return -1;
}

float getLux (byte pin = 36, int Nmeasures = Number_of_measures, float Max_val = 910, float Min_val = 55) {
    // 910 and 55 are empiric values extract while testing the circut
    float lux = 0.0;
    for(int i = 0; i < Nmeasures; i++) {
        lux += (Max_val - (float)analogRead(pin)) / (Max_val - Min_val) * 100;
        //telnet_println("Sample-LUX: " + String(lux));
        delay(25);
    }
	  lux = lux / Nmeasures;
    if ( lux < 0 )   lux = 0.0;
    if ( lux > 100 ) lux = 100.0;
    //telnet_println("LUX: " + String(lux));
    return lux;
}


void ambient_get_data() {
    Temperature = getTemperature();
    Humidity = getHumidity();
    //Lux = getLux();
}

void ambient_send_data() {
    if ( Temperature == -100 ) {
            telnet_print("Temperatura: -- ERRO! -- \t");
            mqtt_publish(mqtt_pathtele(), "Status", "ERRO-Temperatura");
    } else {
            telnet_print("Temperatura: " + String(Temperature) + " C \t");
            mqtt_publish(mqtt_pathtele(), "Temperatura", String(Temperature));
    };

    if ( Humidity == -1 ) {
            telnet_print("Humidade: -- ERRO! -- \t");
            mqtt_publish(mqtt_pathtele(), "Status", "ERRO-Humidade");
    } else {
            telnet_print("Humidade: " + String(Humidity) + " % \t");
            mqtt_publish(mqtt_pathtele(), "Humidade", String(Humidity));
    };
          
    //telnet_print("Lux: " + String(Lux) + " % \t");
    //mqtt_publish(mqtt_pathtele(), "Lux", String(Lux));
    telnet_println("");
}

void ambient_send_json () {
    String temp_Timestamp, temp_BatLevel;
    char fbat[3];        // long enough to hold complete floating string

    // Purge old JSON data and Load new values
    ambient_doc.clear();
    ambient_doc["Timestamp"] = curUnixTime();
    ambient_doc["BatLevel"] = String(dtostrf(getVoltage(),3,0,fbat)).toFloat();
    ambient_doc["RSSI"] = getRSSI();
    ambient_doc["Temperature"] = Temperature;
    ambient_doc["Humidity"] = Humidity;


    serializeJson(ambient_doc, ambient_jsonString);             //Serialize JSON data to string
    // Serial.print("jsonString ready to Publish: "); Serial.println((jsonString));
    telnet_println("Telemetry: " + String(ambient_jsonString));
    mqtt_publish(mqtt_pathtele(), "Telemetry", String(ambient_jsonString));
}

void ambient_setup() {
    if (DHTPIN>=0 || SDAPIN>=0) {
        // Start Ambient Sensor
        if (DHTTYPE == DHT_11 || DHTTYPE == DHT_22) dht_val.begin();       // required if using Adafruit Library
        if (DHTTYPE == AM_2320) {
            //I2C_scan();
            Wire.begin(SDAPIN, SCKPIN);
        }
    }
}

void ambient_data() {
    if (DHTPIN>=0 || SDAPIN>=0) {
        ambient_get_data();
        ambient_send_json();
        //ambient_send_data();
    }
}