#include <ArduinoMqttClient.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <BH1750.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "/home/alejandra/Desktop/IoT-agriculture-measurements/mqtt_bme280_bh1750/net_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.20.25";
int        port     = 1884;


String topics[5] = {"pressure", "temp", "hum", "lux", "awita"};
String deviceId = "AAL";

Adafruit_BME280 bme; // sensor 1  
BH1750 lightMeter(0x23); // sensor 2

int pinRain = A0;
int DigitalRain = 0;


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
  DigitalRain = analogRead(pinRain);

  //mediciones: [presión en hPa, humedad, temperatura, lux, Lluvia]
  double measurement[5] = {bme.readPressure()/100.0F,  bme.readTemperature(), bme.readHumidity(), lightMeter.readLightLevel(), DigitalRain};

  //creación de las payload con los datos a ser enviado a cada tema
  String payloads[5] = {"", "", "", "", ""};

  for(int i = 0; i < 5; i++)
    payloads[i] += measurement[i];
  
  //seteo de las variables para envío del mensaje via protocolo mqtt
    bool retained = false; 
    int qos = 2; 
    bool dup = false;
  
   for(int i = 0; i < 5; i++){
      mqttClient.beginMessage( topics[i]+deviceId, payloads[i].length(), retained, qos, dup);
      mqttClient.print(payloads[i]);
      mqttClient.endMessage();

      Serial.println();
      Serial.print(topics[i]+deviceId);
      Serial.print("/");
      Serial.print(payloads[i]);
      Serial.println();
    }

  delay(5000);
}
