#include <Arduino.h>
#include <SPI.h>
#include <EthernetENC.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress localIp (192,168,2,12);
IPAddress localDns(192,168,2,2);
IPAddress localGw (192,168,2,1);

EthernetServer server(80);
String readString; 

int ledPin = 2;

void setup() 
{ 
  Serial.begin(115200);
  delay(10);
  pinMode(ledPin, OUTPUT); 

  Serial.println("start ethernet");
  Ethernet.begin(mac, localIp, localDns, localGw);
  delay(10);
  Serial.println(Ethernet.localIP());
  Serial.println(Ethernet.dnsServerIP());
  Serial.println(Ethernet.gatewayIP());
  Serial.println("start server");
  server.begin(); 
}

void loop() 
{
  EthernetClient client = server.available(); 

  if (client) 
  { 
    while (client.connected()) 
    { 
      if (client.available()) 
      {
        char c = client.read();
        if (readString.length() < 100) 
        {
          readString += c;
        }
        if (c == 0x0D) 
        {
          client.println("HTTP/1.1 200 OK"); 
          client.println("Content-Type: text/html"); 
          client.println(); 
          client.println("<HTML>"); 
          client.println("<HEAD>"); 
          client.println("<TITLE> ARDUINO ETHERNET</TITLE>"); 
          client.println("/HEAD"); 
          client.println("<BODY>"); 
          client.println("hr"); 
          client.println("H1 style=\"color:green;text-align:center\">ARDUINO ETHERNET LED CONTROL</H1> "); 
          client.println("hr"); 
          client.println("br");
          client.println("<H2 style=\"text-align:centre\"><a href=\"/?LEDON\"\"> Turn On LED</a><br><H2>");
          client.println("<H2 style=\"text-align:center\"><a href=\"/?LEDOFF\"\">Turn Off LED</a><br><H2>");
          client.println("<br>"); 
          client.println("</BODY>"); 
          client.println("</HTML>"); 
          delay(10); 
          client.stop();
          if(readString.indexOf("?LEDON") > -1)
          { 
            digitalWrite(ledPin, HIGH); 
          } 
          else
          { 
            if(readString.indexOf("?LEDOFF") > -1) 
            { 
            digitalWrite(ledPin, LOW); 
            } 
          }
        }
        readString="";
	 	  } 
    } 
  } 
} 
