#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"


char ssid[] = SECRET_SSID;        // WiFi name
char pass[] = SECRET_PASS;        // WiFi password

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "test.mosquitto.org"; 
int port = 1883;                     
const char topic[] = "real_unique_topic";  


#define TRIG_PIN 9
#define ECHO_PIN 10
const long interval = 1000;
unsigned long previousMillis = 0;

void setup() {

  Serial.begin(9600);
  while (!Serial) {
    ; 
  }

  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(5000);
  }
  Serial.println("WiFi connected!");
  Serial.print("Connecting to MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);  
  }

  Serial.println("Connected to MQTT broker!");

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  mqttClient.poll(); 

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    long distance = measureDistance();

    Serial.print("Sending message to topic: ");
    Serial.println(topic);
    Serial.print("Distance: ");
    Serial.println(distance);

    mqttClient.beginMessage(topic);
    mqttClient.print(distance);
    mqttClient.endMessage();

    Serial.println();
  }
}

long measureDistance() {
  
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);

  long distance = (duration / 2) * 0.0343;

  return distance;
}
