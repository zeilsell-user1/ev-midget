#include <Arduino.h>
#include <SPI.h>
#include <EthernetENC.h>
#include <PubSubClient.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF };
IPAddress localIp (192,168,2,21);
IPAddress localDns(8,8,8,8);
IPAddress localGw (192,168,2,1);
IPAddress mqttServer {192, 168, 2, 61};

//EthernetServer webServer(80); // a non-interactive page of current status 
EthernetClient ethClient;
PubSubClient   mqttClient(ethClient);

long lastReconnectAttempt = 0;

const int en = 2;// 0 for enable, 1 for disable
const int s0 = 3; // msb
const int s1 = 4;
const int s2 = 5; 
const int s3 = 6; // lsb
const int sg = 7; // the value of the switch

const int multiplexer[16][4] = {
    {LOW , LOW , LOW , LOW }, // sidelights front and back
    {LOW , LOW , LOW , HIGH}, // headlights dipped 
    {LOW , LOW , HIGH, LOW }, // headlights full
    {LOW , LOW , HIGH, HIGH}, // left indicator front and back
    {LOW , HIGH, LOW , LOW }, // right indicator front and back
    {LOW , HIGH, LOW , HIGH}, // hazard indicators front and back
    {LOW , HIGH, HIGH, LOW }, // brake lights
    {LOW , HIGH, HIGH, HIGH}, // front fog light
    {HIGH, LOW , LOW , LOW }, // rear fog light
    {HIGH, LOW , LOW , HIGH}, // front spot light
    {HIGH, LOW , HIGH, LOW }, // reversing light
    {HIGH, LOW , HIGH, HIGH}, // flasher (instantaneous full and spot)
    {HIGH, HIGH, LOW , LOW }, // windscreen washers
    {HIGH, HIGH, LOW , HIGH}, // windscreen wipers
    {HIGH, HIGH, HIGH, LOW }, // blower
    {HIGH, HIGH, HIGH, HIGH}, // spare
};

int state[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

const char* events[16] = {
    "topic/lights/side",
    "topic/lights/head-dipped",
    "topic/lights/head-full",
    "topic/lights/left-indicator",
    "topic/lights/right-indicator",
    "topic/lights/hazards",
    "topic/lights/brakes",
    "topic/lights/front-fog",
    "topic/lights/rear-fog",
    "topic/lights/spot",
    "topic/lights/reversing",
    "topic/lights/flasher",
    "topic/windscreen/washer",
    "topic/windscreen/wiper",
    "topic/heating/blower",
    "topic/spare"
};

void onSwitchChangeState(int channel, const char *value)
{
  mqttClient.publish(events[channel], value);
  char str[80];
  sprintf(str, "--> %s %s", events[channel], value);
  Serial.println(str);
}

void checkSwitchStates() 
{
  int channel = 0;
  int value = 0;

  Serial.println("check switch states");

  while (channel < 16)
  {
    Serial.print(channel);
    Serial.print(",");
    /*
    digitalWrite(en, HIGH);
    digitalWrite(s0, multiplexer[channel][3]);
    digitalWrite(s1, multiplexer[channel][2]);
    digitalWrite(s2, multiplexer[channel][1]);
    digitalWrite(s3, multiplexer[channel][0]);
    digitalWrite(en, LOW);
    value = digitalRead(sg);
    */
/*
    if (state[channel] != value)
    {
      Serial.println("state needs updating");
      char str[8];
      itoa( value, str, 10 );
      onSwitchChangeState(channel, str);
      state[channel] = value;
    }
*/
    channel += 1;
  }
  Serial.println("... fin");
}

void onMqttMessageReceived(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i=0; i < length; i++) 
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
} 

/* void displayServerPage()
{ 
  EthernetClient webClient = webServer.available(); 

  if (webClient) 
  { 
    if (webClient.connected()) 
    { 
      webClient.println("HTTP/1.1 200 OK"); 
      webClient.println("Content-Type: text/html"); 
      webClient.println(); 
      webClient.println("<HTML>"); 
      webClient.println("<HEAD>"); 
      webClient.println("<TITLE> HHW470D Switch Status</TITLE>"); 
      webClient.println("/HEAD"); 
      webClient.println("<BODY>"); 
      webClient.println("<H1>Switch Status</H1>"); 
      webClient.println("<br>"); 
      webClient.println("</BODY>"); 
      webClient.println("</HTML>"); 
      webClient.stop();
    } 
  } 
} */

void setup() 
{ 
  Serial.begin(115200);
  delay(10);

  Serial.println("configure the hardware");

  pinMode(en, OUTPUT);
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(sg, INPUT_PULLUP);

delay(200);
  Serial.println("start ethernet");

  Ethernet.begin(mac, localIp, localDns, localGw);
  delay(10);
  Serial.println(Ethernet.localIP());
  Serial.println(Ethernet.dnsServerIP());
  Serial.println(Ethernet.gatewayIP());

  //Serial.println("start web server");
  //webServer.begin(); 

  Serial.println("configure the MQTT server");

  ethClient.setTimeout(6000);
  mqttClient.setServer(mqttServer, 1883);
  mqttClient.setSocketTimeout(6); // annoyingly this is in seconds
  mqttClient.setKeepAlive(2); // annoyingly this is in seconds
  mqttClient.setCallback(onMqttMessageReceived);

  // delay 5 seconds to allow the MQTT broker to come up and get ready to esablish 
  delay(5000);
}

void loop() 
{
  //displayServerPage();

  // if not connected to the client the
  if (!mqttClient.connected()) 
  {
    mqttClient.disconnect(); // send a disconnect to make sure that it is clean start
    ethClient.stop();
    
    delay(2000);

    Serial.print("connect to MQTT Broker ....");

    if (mqttClient.connect("DashClient",NULL,NULL,"connect",0,false,"connecting",true)) 
    {
      Serial.println("connect returned true");
      mqttClient.subscribe("topic/connect");
      mqttClient.publish("topic/connect","Switches connected"); 
    }
    else
    {
      Serial.println("connect returned fail");
      delay(20000); // do nothing for 20 seconds and try connection again
    }
  } 
  else // Client connected
  {
    Serial.print(".");
    mqttClient.loop(); // not a loop! this is the check for new messages
    delay(200);
    checkSwitchStates();
    //Ethernet.maintain();
  } 
} 
