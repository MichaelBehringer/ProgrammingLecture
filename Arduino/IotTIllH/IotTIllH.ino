#include <ArduinoMqttClient.h>
#include <Wire.h>
#include "Adafruit_ADT7410.h"
#include <Adafruit_LSM6DS33.h>
#include "arduino_secrets.h"
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>


///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS; 

// Definiere die Pin-Nummer für den Photowiderstand
const int sensorPin = A0;

// Create the ADT7410 temperature sensor object
Adafruit_ADT7410 tempsensor = Adafruit_ADT7410();
Adafruit_LSM6DS33 lsm6ds33;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "ai.tillh.de";
int        port     = 1883;
const String topic  = "/dhai/Wemding/Behringer/";

const long interval = 5000;
unsigned long previousMillis = 0;

int sequence = 0;

void setup() {
  Serial.begin(9600);

  doWifiConnect();
  doMqttSetup();
  doMqttConnect();
  doSensorSetup();
}

void sendMqtt(int sequence, String topic, float value, String unit) {
  String jsonMsg = "{\"value\": ";
  jsonMsg += value;
  jsonMsg += ", \"unit\": ";
  jsonMsg += unit;
  jsonMsg += ", \"sequence\": ";
  jsonMsg += sequence;
  jsonMsg += "}";

  Serial.print("Sending message to topic: ");
  Serial.println(topic);
  Serial.println(jsonMsg);

  mqttClient.beginMessage(topic);
  mqttClient.print(jsonMsg);
  mqttClient.endMessage();
}

void doWifiConnect() {
  Serial.println("Start connecting to WiFi.");
  if (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(5000);
  }
  Serial.println("Connected to WiFi.");
}

void doMqttSetup() {
  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  mqttClient.setId("4264");

  // You can provide a username and password for authentication
  mqttClient.setUsernamePassword("behringer", "Bcdrf6.x");
}

void doMqttConnect() {
  Serial.println("Start connecting to MQTT Broker.");
  if (!mqttClient.connect(broker, port)) {
    Serial.print(".");
    delay(5000);
  }
  Serial.println("Connected to MQTT Broker.");
}

void doSensorSetup() {
  Serial.println("Adafruit LSM6DS33 Test");

  if (!lsm6ds33.begin_I2C()) {
    Serial.println("LSM6DS33 nicht gefunden");
    while (1);
  }
  Serial.println("LSM6DS33 gefunden!");


  // Make sure the sensor is found, you can also pass in a different i2c
  // address with tempsensor.begin(0x49) for example
  if (!tempsensor.begin()) {
    Serial.println("Couldn't find ADT7410!");
    while (1);
  }

  // sensor takes 250 ms to get first readings
  delay(250);

  tempsensor.setResolution(ADT7410_16BIT);
}

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();

  unsigned long currentMillis = millis();

 

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Read light sensor value
    sensors_event_t accel;
    sensors_event_t gyro_not_in_use;
    sensors_event_t temp_not_in_use;
    lsm6ds33.getEvent(&accel, &gyro_not_in_use, &temp_not_in_use);

    float accY = 0;
    float temp = 0;
    for(int i = 0; i < 20; i++) {
      lsm6ds33.getEvent(&accel, &gyro_not_in_use, &temp_not_in_use);
      accY += accel.acceleration.y;
      temp += tempsensor.readTempC();
      delay(5);
    }
    accY = abs(accY / 20);
    accY = accY > 9.81 ? 9.81 : accY;
    float angle = accY > 9.81 ? 90.0 : asin(accY/9.81)*180/3.1415;

    temp = temp / 20;

    sendMqtt(sequence, topic + "temp", temp, "C");
    sendMqtt(sequence, topic + "angle", angle, "°");

    sequence++;
  }
}
