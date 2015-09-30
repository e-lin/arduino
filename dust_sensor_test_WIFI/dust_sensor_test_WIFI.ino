#define ETHERNET_TEST 0
#define WIFI_TEST 1
#define MQTT_TEST 1
#define AUTH 1

#include <SPI.h>
#if ETHERNET_TEST
#include <Ethernet2.h>
#endif
#if WIFI_TEST
#include <SFE_CC3000.h>
#include <SFE_CC3000_Client.h>
#endif
#if AUTH
#include "Authentication_OneFile.h"
#include <sha1.h>
#include <Base64.h>
#endif
#if MQTT_TEST
#include <PubSubClient.h>
#endif


#if AUTH
#define CONSUMER_KEY "magellanop.Project1"
#define CONSUMER_SECRET "mysecret"
#define CLIENT_VERSION "0.0.1"
char* password;
#endif
#if MQTT_TEST
//#define serverName "nebula-001a-mqtt.magellanic-clouds.net" //104.155.231.238
//IPAddress serverName(104,155,231,238);
//#define serverName "nebula-staginga-mqtt.staging-magellanic-clouds.net" //104.155.217.86
//#define serverName "test.mosquitto.org" //85.119.83.194
IPAddress serverName(192,168,1,153);
#define clientName "7ff70fd2a290"
#define topicName "hello/topic"
#endif

#if ETHERNET_TEST
byte mac[] = { 0x90, 0xA2, 0xD2, 0x0F, 0xF7, 0x7F };
EthernetClient mqttClient;
#endif

#if WIFI_TEST
// Pins
#define CC3000_INT      2   // Needs to be an interrupt pin (D2/D3)
#define CC3000_EN       7   // Can be any digital pin
#define CC3000_CS       10  // Preferred is pin 10 on Uno

// Connection info data lengths
#define IP_ADDR_LEN     4   // Length of IP address in bytes

// Constants
char ap_ssid[] = "Elsa's iphone";                  // SSID of network
char ap_password[] = "marina99";          // Password of network
unsigned int ap_security = WLAN_SEC_WPA2; // Security of network
unsigned int timeout = 30000;             // Milliseconds
//char server[] = "www.example.com";        // Remote host site

// Global Variables
SFE_CC3000 wifi = SFE_CC3000(CC3000_INT, CC3000_EN, CC3000_CS);
SFE_CC3000_Client mqttClient = SFE_CC3000_Client(wifi);
#endif

#if MQTT_TEST
PubSubClient client( mqttClient );
#endif

//dust sensor related
int measurePin = 0;  //connect dust sensor to Arduino A0 pin
int ledPower = 9;   //connect 3rd led driver pins of dust sensor to Arduino D9

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
  
float voMeasured = 0.0;
float calcVoltage = 0.0;
float dustDensity = 0.0;

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
  
  Serial.print(F("Raw Signal Value (0-1023): "));
  Serial.print(voMeasured);
  
  Serial.print(F(" - Voltage: "));
  Serial.print(calcVoltage);
  
  Serial.print(F(" - Dust Density: "));
  Serial.println(dustDensity); // unit: mg/m3
}

char* buildJson(){
  char data[200];
  
  int v1 = voMeasured;
  float tmp = voMeasured - v1;
  int v2 = (int)(tmp*100);
  
  int c1 = calcVoltage;
  tmp = calcVoltage - c1;
  int c2 = (int)(tmp*100);
  
  int d1 = dustDensity;
  tmp = dustDensity - d1;
  int d2 = (int)(tmp*100);
  
  sprintf(data,"{\n\"Dust Sensor\": \"Sharp GP2Y1010AU0F\",\n\"Raw Signal\": %d.%04d,\n\"Voltage\": %d.%04d,\n\"Dust Density\": %d.%04d\n}", v1, v2, c1, c2, d1, d2);
  
  return data;
}

#if MQTT_TEST
void publishData() {
  
  //get dust sensor data
  getData();

  char* json = buildJson();
  boolean pubresult = client.publish(topicName, json);
//  boolean pubresult = client.publish(topicName,"Hi Peggy");
    
  Serial.print(F("attempt to send "));
  Serial.println(json);
  Serial.print(F("to "));
  Serial.println(topicName);
    
  if (pubresult){
    Serial.println(F("successfully sent"));
  }else{
    Serial.println(F("unsuccessfully sent"));
  }
}

void connect(){
  while(!client.connected()){
    Serial.println(F("Attempting MQTT conncection..."));
//    Serial.print(F("username: "));
//    Serial.println(CONSUMER_KEY);

//      if(!client.connect(clientName)){
//    if(!client.connect(clientName, CONSUMER_KEY, password )){
  if(!client.connect("7ff70fd2a290", "magellanop.Project1", "123" )){
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      Serial.println(F(" try again in 5 seconds"));
      delay(5000);
    }
  }
}
#endif

void setup(){  
  Serial.begin(115200);
  pinMode(ledPower,OUTPUT);
  
#if MQTT_TEST
  client.setServer(serverName, 1883);
#endif

#if ETHERNET_TEST
  Serial.print(F("Initializing Ethernet..."));
  if(Ethernet.begin(mac)) {
    Serial.print(F("Connected with IP: "));
    Serial.println(Ethernet.localIP());
  }
#endif

#if WIFI_TEST
  ConnectionInfo connection_info;
  int i;
  
  Serial.print(F("Initializing SparkFun CC3000 - WebClient..."));    
  // Initialize CC3000 (configure SPI communications)
  if ( wifi.init() ) {
    Serial.println("CC3000 initialization complete");
  } else {
    Serial.println("Something went wrong during CC3000 init!");
  }
  
  // Connect using DHCP
  Serial.print("Connecting to SSID: ");
  Serial.println(ap_ssid);
  if(!wifi.connect(ap_ssid, ap_security, ap_password, timeout)) {
    Serial.println("Error: Could not connect to AP");
  }
  
  // Gather connection details and print IP address
  if ( !wifi.getConnectionInfo(connection_info) ) {
    Serial.println("Error: Could not obtain connection details");
  } else {
    Serial.print("IP Address: ");
    for (i = 0; i < IP_ADDR_LEN; i++) {
      Serial.print(connection_info.ip_address[i]);
      if ( i < IP_ADDR_LEN - 1 ) {
        Serial.print(".");
      }
    }
    Serial.println();
  }
#endif

#if AUTH
  Authentication auth( CONSUMER_KEY, CONSUMER_SECRET, CLIENT_VERSION );
  password = auth.getPassword();
  Serial.println(strlen(password));
  Serial.println(password);
#endif

  connect(); //must connect in setup
}

void loop(){
  
#if MQTT_TEST  
  if(!client.connected()){
    Serial.println(F("Reconnecting..."));
    connect();
  }else{
    Serial.println(F("Publish data..."));
    publishData();
  }
  
  client.loop();
  delay(5000);
#endif
}
