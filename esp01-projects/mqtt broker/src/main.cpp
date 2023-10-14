#include <Arduino.h>
#include <uMQTTBroker.h>
#include <ESP8266WiFi.h>

const char *ssid = "hhw470d";
const char *password = "Target21#";

uMQTTBroker myBroker;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();

  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP()); // Print the IP address

  // Start the broker
  Serial.println("Starting MQTT broker");
  myBroker.init();
}

void loop() {
  // do anything here
  delay(1000);
}