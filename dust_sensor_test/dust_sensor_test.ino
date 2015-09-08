#define ETHERNET_TEST 1
#define MQTT_TEST 1
#define AUTH 1

#include <SPI.h>
#if ETHERNET_TEST
#include <Ethernet2.h>
//#include <EthernetUdp2.h>
//#include <Dns.h>
#endif
#if AUTH
#include "Authentication_OneFile.h"
#include <sha1.h>
#include <Base64.h>
#endif
#if MQTT_TEST
#include <PubSubClient.h>
#endif
#include <MemoryFree.h>
#define DEBUG 1

#if ETHERNET_TEST
byte mac[] = { 0x90, 0xA2, 0xD2, 0x0F, 0xF7, 0x7F };
EthernetClient ethClient;
#endif
#if MQTT_TEST
PubSubClient client( ethClient );
void reconnect(){
  while(!client.connected()){
    Serial.println(F("Attempting MQTT conncection..."));
    
    //if(client.connect(clientName, CONSUMER_KEY, password)){
    if(client.connect(clientName)){
      
      boolean pubresult = client.publish(topicName, "Hi Elsa");
      if (pubresult){
        Serial.println(F("successfully sent"));
      }else{
        Serial.println(F("unsuccessfully sent"));
      }
    }else{
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      Serial.println(F(" try again in 5 seconds"));
      delay(5000);
    }
  }
}
#endif

#if DEBUG
int free_ram()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
#endif

void setup(){  
  Serial.begin(115200);
#if DEBUG
  Serial.println(F("memory(start): "));
  Serial.println( freeMemory());
  Serial.println( free_ram());
#endif

#if MQTT_TEST
client.setServer(serverName, 1883);
#endif

#if ETHERNET_TEST
  Serial.print(F("Initializing Ethernet..."));
  //byte mac[] = { 0x90, 0xA2, 0xD2, 0x0F, 0xF7, 0x7F };
  if(Ethernet.begin(mac)) {
    Serial.print(F("Connected with IP: "));
    Serial.println(Ethernet.localIP());
  }
#endif
#if DEBUG
  Serial.println(F("memory: "));
  Serial.println( freeMemory());
  Serial.println( free_ram());
#endif

#if AUTH
  Authentication auth( CONSUMER_KEY, CONSUMER_SECRET, CLIENT_VERSION );
  password = auth.getPassword();
  Serial.println(strlen(password));
  Serial.println(password);
#endif

#if DEBUG
  Serial.println(F("memory: "));
  Serial.println( freeMemory());
  Serial.println( free_ram());
#endif

  //allow the hardware to sort itself out
  delay(1500);
}

void loop(){
#if DEBUG
  Serial.println(F("memory(loop): "));
  Serial.println( freeMemory());
  Serial.println( free_ram());
  Serial.println(strlen(password));
  Serial.println(password);
#endif

#if MQTT_TEST
  if(!client.connected()){
    reconnect();
  }
  client.loop();
#endif

#if DEBUG
  Serial.println(F("memory: "));
  Serial.println( freeMemory());
  Serial.println( free_ram());
#endif
  delay(5000);
}
