/*
        Send Dust Sensor Data to IBM IoT Foundation via MQTT

        - Continuously obtains data from Sharp Optical Dust Sensor GP2Y1010AU0F
        - Formats the data as a JSON string
        - Connects to a MQTT server (either local or IBM IoT Foundation)
        - Publishes the JSON string to a topic
        
        Reference:
        http://www.ibm.com/developerworks/cloud/library/cl-bluemix-arduino-iot2/#download

	The circuit:
	http://arduinodev.woofex.net/2012/12/01/standalone-sharp-dust-sensor/

	Created 06/23/2015 By Yijhen Lin
*/
#include <SPI.h>
#include <Ethernet2.h>
#include <PubSubClient.h>
#include "Authentication_OneFile.h"
#include <sha1.h>
#include <Base64.h>

//MAC address of the Ethernet shield
byte mac[] = { 0x90, 0xA2, 0xD2, 0x0F, 0xF7, 0x7F };
char macstr[] = "90a2d20ff77f";

//for local testing, a local MQTT server and an IP on local network
byte localserver[] = { 192, 168, 1, 148 };

//for IBM IoT Foundation
//char servername[]="messaging.quickstart.internetofthings.ibmcloud.com";
//char servername[]="nebula-001a-mqtt.magellanic-clouds.net";
//#define CONSUMER_KEY "nac8c0x8w7om54pz"
//#define CONSUMER_SECRET "df2ku6pghf5dr4qs346hrnrplbx6w1au"
//#define CLIENT_VERSION "DefaultStage1"
char servername[]="nebula-staginga-mqtt.staging-magellanic-clouds.net";
#define CONSUMER_KEY "r8a7gooirs1mwx62"
#define CONSUMER_SECRET "j54cd6iotnbfji2ua0m6rs7qhmjfuqqr"
#define CLIENT_VERSION "DefaultStage1"
char* password;
String clientName = String("d:quickstart:arduino:") + macstr;
String topicName = String("iot-2/evt/status/fmt/json");

int measurePin = 0;  //connect dust sensor to Arduino A0 pin
int ledPower = 9;   //connect 3rd led driver pins of dust sensor to Arduino D9

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
  
float voMeasured = 0.0;
float calcVoltage = 0.0;
float dustDensity = 0.0;

//initialize the Ethernet client
EthernetClient ethClient;

//Uncomment this next line and comment out the line after it to test against a local MQTT server
//PubSubClient client(localserver, 1883, 0, ethClient);
PubSubClient client(servername, 1883, 0, ethClient);

void setup(){

  Serial.begin(115200);
  pinMode(ledPower,OUTPUT);
  
  if( 0 == Ethernet.begin(mac) ){
    Serial.println("Failed to configue Ethernet using DHCP");
  }
  
  //print the local IP address
  Serial.print("My IP address:");
  for( byte thisByte = 0; thisByte < 4; thisByte++ ){
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println();
  
  Authentication auth( CONSUMER_KEY, CONSUMER_SECRET, CLIENT_VERSION );
  password = auth.getPassword();
  Serial.println(strlen(password));
  Serial.println(password);
}
  
void loop(){
  char clientStr[34];
  clientStr[33] = '\0';
  clientName.toCharArray(clientStr,34);
  char topicStr[26];
  topicName.toCharArray(topicStr,26);
  
  //get dust sensor data
  getData();
  
  if (!client.connected()) {
    Serial.println(F("Attempting MQTT conncection..."));
    //client.connect(clientStr);
    if(client.connect(clientStr, CONSUMER_KEY, password )){
      Serial.println(F("Connected"));
    }else{
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
    }
  }
  if (client.connected() ) {
    String json = buildJson();
    char jsonStr[200];
    json.toCharArray(jsonStr,200);
    
    //publish data
    boolean pubresult = client.publish(topicStr,jsonStr);
    
    Serial.print("attempt to send ");
    Serial.println(jsonStr);
    Serial.print("to ");
    Serial.println(topicStr);
    
    if (pubresult){
      Serial.println("successfully sent");
    }else{
      Serial.println("unsuccessfully sent");
    }
  }
  delay(5000);
}

String buildJson() {
  String data = "{";
  data+="\n";
  data+= "\"d\": {";
  data+="\n";
  data+="\"Dust Sensor\": \"Sharp GP2Y1010AU0F\",";
  data+="\n";
  data+="\"Raw Signal\": ";
  data+=voMeasured;
  data+= ",";
  data+="\n";
  data+="\"Voltage\": ";
  data+=calcVoltage;
  data+= ",";
  data+="\n";
  data+="\"Dust Density (mg\/m3)\": ";
  data+=dustDensity;
  data+="\n";
  data+="}";
  data+="\n";
  data+="}";
  return data;
}

void getData(){
  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(samplingTime);
  
  voMeasured = analogRead(measurePin); // read the dust value
  
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);
  
  // 0 - 5V mapped to 0 - 1023 integer values
  // recover voltage
  calcVoltage = voMeasured * (5.0 / 1024.0);
  
  // linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
  // Chris Nafis (c) 2012
  dustDensity = 0.17 * calcVoltage - 0.1;
  
  Serial.print("Raw Signal Value (0-1023): ");
  Serial.print(voMeasured);
  
  Serial.print(" - Voltage: ");
  Serial.print(calcVoltage);
  
  Serial.print(" - Dust Density: ");
  Serial.println(dustDensity); // unit: mg/m3
}
