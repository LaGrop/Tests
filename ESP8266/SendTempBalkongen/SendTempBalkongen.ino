#include <dummy.h>

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2 // DS18B20 on arduino pin2 corresponds to D4 on physical board

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
WiFiClient client;

#define DEEP_SLEEP 120e6
//#define DEEP_SLEEP 5e6

String apiKey = "BMIHVI07ZPPI0ACC";
const char* ssid = "Altitude110m";
const char* password =  "topfloor@110m";
const char* server = "api.thingspeak.com";

void setup() {
  int retries=10;
  
  Serial.begin(115200);
  delay(10);
  
  WiFi.begin(ssid, password);
  
  Serial.println();
  Serial.println("Connecting");
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    retries--;
    if (retries==0)
    {
      Serial.println("Connect failed!. Going to deep sleep.");
      ESP.deepSleep(60e6);
    }
    delay(1000);
  }
  
  Serial.println("WiFi Connected");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  float temp1;
  float temp2=0;
  
  DS18B20.requestTemperatures();
  
  temp1 = DS18B20.getTempCByIndex(0);
  // temp2 = DS18B20.getTempCByIndex(1);

  if ((temp1 < 55) && (temp2 < 55) && client.connect(server,80)) {
    String postStr = apiKey;
    // postStr +="&field1=";
    // postStr += String(temp1);
    // postStr +="&field2=";
    // postStr += String(temp2);
    postStr +="&field1=";
    postStr += String(temp1);
    //postStr += "\r\n\r\n";
    
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    
    Serial.print("Temperature 1: ");
    Serial.println(temp1);
    // Serial.print("Temperature 2: ");
    // Serial.println(temp2);  
  }
  client.stop();

  Serial.println("Going to deep sleep.");
  ESP.deepSleep(DEEP_SLEEP);
  //delay(900000); // 900 s = 15 min delay between updates
}
