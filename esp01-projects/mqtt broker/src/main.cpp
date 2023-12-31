#include <Arduino.h>
#include <uMQTTBroker.h>
#include <ESP8266WiFi.h>
#include "mqtt/defaults.h"

const char *ssid = "hhw470d";
const char *password = "Target21#";

IPAddress localIp(192, 168, 2, 61);
IPAddress localGw(192, 168, 2, 1);
IPAddress localDns(192, 168, 2, 2); 
IPAddress subnet(255, 255, 0, 0);

uMQTTBroker myBroker((uint16_t)MQTT_BROKER_PORT, (uint8_t)MAX_SUBSCRITIONS, (uint8_t)MAX_RETAINED_TOPICS);

void setup() {
  Serial.begin(115200);
  delay(10);
  
  WiFi.mode(WIFI_STA);
  
  if (!WiFi.config(localIp, localGw, subnet, localDns)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP()); // Print the IP address

  // Start the broker
  Serial.println("Starting MQTT broker");
  myBroker.init();
}

void loop() {
  Serial.print(".");
  delay(200);
}