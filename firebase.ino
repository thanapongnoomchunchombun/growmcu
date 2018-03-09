//include//
#include <ArduinoJson.h>
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <EEPROM.h>
//end//
  
//define//
#define FIREBASE_HOST "hydropronic-esp8266.firebaseio.com"
#define FIREBASE_KEY "kACFssqG6Vp1nsyWq6Z1gyTp1wgOJrGnXNeKj8kn"
#define temp_humid D2
#define moisture D3
#define pumpwater D4
#define light D5
#define lightgrow D6
#define ESP_AP_NAME "BasketOfGrow"
#define ledPin D7
#define ConfigWiFi_Pin D1 
//end//

const String deviceNumber = "120";
DHT dht;
int lightaddr = 0;
int lightgrowaddr = 1;
int pumpaddr = 2;

void setup()
{

    // initialize pin D0 as an output.
  pinMode(ledPin, OUTPUT);
  pinMode(ConfigWiFi_Pin,INPUT_PULLUP);

  Serial.begin(115200);
  digitalWrite(ledPin,LOW);//Turn on the LED
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  if(digitalRead(ConfigWiFi_Pin) == LOW) // Press button
  {
    //reset saved settings
    wifiManager.resetSettings(); // go to ip 192.168.4.1 to config
  }
  //fetches ssid and password from EEPROM and tries to connect
  //if it does not connect, it starts an access point with the specified name
  //and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect(ESP_AP_NAME); 
  while (WiFi.status() != WL_CONNECTED) 
  {
     delay(250);
     Serial.print(".");
  }
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(ledPin,HIGH);

  dht.setup(temp_humid); // temp-humid data
  Firebase.begin(FIREBASE_HOST, FIREBASE_KEY);
  EEPROM.begin(512);
   
}

void loop()
{
  //module tempereture/humidity
  delay(dht.getMinimumSamplingPeriod());
  int humidity = dht.getHumidity();
  int temperature = dht.getTemperature();
  //end

  //module soil
  int soil = analogRead(soil);
  //end

  //Data JSON object (send)
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& valueObject = jsonBuffer.createObject();
  valueObject["AirTemp"] = temperature;
  valueObject["AirHumid"] = humidity;
  valueObject["Soil"] = soil;
  //end

  //pust data to fisebase (object)
  Firebase.set(deviceNumber + "/value/", valueObject);
  delay(1000);
  //end

  //get data from fisebase
  String lightfirebase = Firebase.getString("/device/light");
  String lightgrowfirebase = Firebase.getString("/device/lightgrow");
  String pumpfirebase = Firebase.getString("/device/pumpwater");
  
  //end

  //get time from firebase
  String lightfirebase = Firebase.getString("/device/light/");
  String lightgrowfirebase = Firebase.getString("/device/lightgrow/");
  String pumpfirebase = Firebase.getString("/device/pumpwater/");
  //end

  
  //condition data save to EEPROM
  int lightEEPROM = EEPROM.read(lightaddr);
  int lightgrowEEPROM = EEPROM.read(lightgrowaddr);
  int pumpEEPROM = EEPROM.read(pumpaddr);
   
  if(lightfirebase != lightEEPROM){
      EEPROM.update(lightaddr,lightfirebase);
    }
  if(lightgrowfirebase != lightgrowEEPROM){
      EEPROM.update(lightgrowaddr,lightgrowfirebase);
    }
  if(pumpfirebase != pumpEEPROM){
      EEPROM.update(pumpaddr,pumpfirebase);
    }

    //On-Off realtime
  if(lightEEPROM = "on"){
    digitalWrite(light,HIGH);
    }else{
    digitalWrite(light,LOW);
      }
  if(lightgrowEEPROM = "on"){
    digitalWrite(lightgrow,HIGH);
    }else{
    digitalWrite(lightgrow,LOW);  
      }
  if(pumpEEPROM = "on"){
    digitalWrite(pumpwater,HIGH);
    }else{
     digitalWrite(pumpwater,LOW);
      }
  //  EEPROM.write(0,100);
  //  int val = EEPROM.read(addr);
  //  Serial.println(EEPROM.read(0));
  //  delay(1000);
  
}

