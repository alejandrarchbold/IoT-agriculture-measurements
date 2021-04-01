#include <ArduinoMqttClient.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <BH1750.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "/home/alejandra/Desktop/mqtt_bme280_bh1750.ino/net_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.1.106";
int        port     = 1883;


String topics[5] = {"pressure", "temp", "hum", "lux"};
String deviceId = "AAL";

Adafruit_BME280 bme;  
BH1750 lightMeter(0x23);

void setup(){
  Serial.begin(9600);

  Wire.begin();
  
  while (!Serial) {
    ;
  }


  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  mqttClient.setUsernamePassword("aaatest1", "aaatest1_nuncasupecomoseescribe");


  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();


  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  }
  else {
    Serial.println(F("Error initialising BH1750"));
  }

  bool status = bme.begin(0x76);
  
}

void loop() {
  mqttClient.poll();

    //mediciones: [presión en hPa, humedad, temperatura, lux, calidad_aire]
    double measurement[5] = {bme.readPressure()/100.0F,  bme.readTemperature(), bme.readHumidity(), lightMeter.readLightLevel()};

    //creación de las payload con los datos a ser enviado a cada tema
    String payloads[5] = { "", "", "", "" };

    for(int i = 0; i < 4; i++)
        payloads[i] += measurement[i];
  
        //seteo de las variables para envío del mensaje via protocolo mqtt
    bool retained = false; 
    int qos = 2; 
    bool dup = false;
  
    for(int i = 0; i < 4; i++){
      mqttClient.beginMessage( topics[i]+deviceId, payloads[i].length(), retained, qos, dup);
      mqttClient.print(payloads[i]);
      mqttClient.endMessage();

      Serial.println();
      Serial.print(topics[i]+deviceId);
      Serial.print("/");
      Serial.print(payloads[i]);
      Serial.println();
      }

    if (lightMeter.measurementReady()) {
    float lux = lightMeter.readLightLevel();
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx");
    }

  delay(5000);
}
