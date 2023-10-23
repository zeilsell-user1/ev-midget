#include <Arduino.h>
#include <uMQTTBroker.h>
#include <ESP8266WiFi.h>

const char *ssid = "hhw470d";
const char *password = "Target21#";

IPAddress localIp(192, 168, 2, 61);
IPAddress localGw(192, 168, 2, 1);
IPAddress localDns(192, 168, 2, 2); 
IPAddress subnet(255, 255, 0, 0);

/*
 * Custom broker class with overwritten callback functions
 */
class myMQTTBroker: public uMQTTBroker
{
public:
    virtual bool onConnect(IPAddress addr, uint16_t client_count) {
      Serial.println("\n\r"+addr.toString()+" connected");
      return true;
    }

    virtual void onDisconnect(IPAddress addr, String client_id) {
      Serial.println("\n\r"+addr.toString()+" ("+client_id+") disconnected");
    }

    virtual bool onAuth(String username, String password, String client_id) {
      Serial.println("\n\rUsername/Password/ClientId: "+username+"/"+password+"/"+client_id);
      return true;
    }
    
    virtual void onData(String topic, const char *data, uint32_t length) {
      char data_str[length+1];
      os_memcpy(data_str, data, length);
      data_str[length] = '\0';
      
      Serial.println("\n\rreceived topic '"+topic+"' with data '"+(String)data_str+"'");
      //printClients();
    }

    // Sample for the usage of the client info methods

    virtual void printClients() {
      for (int i = 0; i < getClientCount(); i++) {
        IPAddress addr;
        String client_id;
         
        getClientAddr(i, addr);
        getClientId(i, client_id);
        Serial.println("Client "+client_id+" on addr: "+addr.toString());
      }
    }
};

myMQTTBroker myBroker;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  
  WiFi.mode(WIFI_STA);
  
  if (!WiFi.config(localIp, localGw, subnet, localDns)) {
    Serial.println("STA Failed to configure");
  }

  Serial.println(WiFi.macAddress());

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
  myBroker.subscribe("#");
}

void loop() {
  Serial.print(".");
  delay(200);
}