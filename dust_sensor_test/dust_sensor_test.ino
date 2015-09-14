#define ETHERNET_TEST 1
#define MQTT_TEST 1
#define AUTH 1
#define OPTIMIZE 0 //failed

#include <SPI.h>
#if ETHERNET_TEST
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <Dns.h>
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


#if OPTIMIZE
  const PROGMEM char
    CONSUMER_KEY[] = "nac8c0x8w7om54pz",
    CONSUMER_SECRET[] = "df2ku6pghf5dr4qs346hrnrplbx6w1au",
    CLIENT_VERSION[] = "DefaultStage1",
    serverName[] = "test.mosquitto.org",
    clientName[] = "7ff70fd2a290",
    topicName[] = "hello/topic";
  const char* const info_table[] PROGMEM = { CONSUMER_KEY, CONSUMER_SECRET, CLIENT_VERSION, serverName, clientName, topicName };
  #define INFO_KEY 0
  #define INFO_SECRET 1
  #define INFO_VERSION 2
  #define INFO_SERVER 3
  #define INFO_CLIENT 4
  #define INFO_TOPIC 5
  char* password;
#else
  #if AUTH
  #define CONSUMER_KEY "nac8c0x8w7om54pz"
  #define CONSUMER_SECRET "df2ku6pghf5dr4qs346hrnrplbx6w1au"
  #define CLIENT_VERSION "DefaultStage1"
  char* password;
  #endif
  #if MQTT_TEST
  #define serverName "nebula-001a-mqtt.magellanic-clouds.net" //104.155.231.238
  //#define serverName "test.mosquitto.org" //85.119.83.194
  #define clientName "7ff70fd2a290"
  #define topicName "hello/topic"
  #endif
#endif

#if ETHERNET_TEST
byte mac[] = { 0x90, 0xA2, 0xD2, 0x0F, 0xF7, 0x7F };
EthernetClient ethClient;
#endif
#if MQTT_TEST
PubSubClient client( ethClient );
void reconnect(){
  while(!client.connected()){
    Serial.println(F("Attempting MQTT conncection..."));
    
    #if OPTIMIZE
    char buf_client[ strlen_P(info_table[INFO_CLIENT]) + 1 ];
    memset( buf_client, 0, strlen_P(info_table[INFO_CLIENT]) + 1 );
    strcpy_P( buf_client, (char*)pgm_read_word(&info_table[INFO_CLIENT]) );
    char buf_topic[ strlen_P(info_table[INFO_TOPIC]) + 1 ];
    memset( buf_topic, 0, strlen_P(info_table[INFO_TOPIC]) + 1 );
    strcpy_P( buf_topic, (char*)pgm_read_word(&info_table[INFO_TOPIC]) );
      
    if(client.connect(buf_client)){    
    #else
    //if(client.connect(clientName)){    
      if(client.connect(clientName, CONSUMER_KEY, password )){    
    #endif
      #if OPTIMIZE
      boolean pubresult = client.publish(buf_topic, "Hi Elsa");
      #else
      boolean pubresult = client.publish(topicName, "Hi Elsa");
      #endif
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
  #if OPTIMIZE
  char buf_server[ strlen_P(info_table[INFO_SERVER]) + 1 ];
  memset( buf_server, 0, strlen_P(info_table[INFO_SERVER]) + 1 );
  strcpy_P( buf_server, (char*)pgm_read_word(&info_table[INFO_SERVER]) );
  Serial.print(F("server: "));
  Serial.println(buf_server);
  client.setServer(buf_server, 1883);
  #else
  client.setServer(serverName, 1883);
  #endif
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
  #if OPTIMIZE
    char buf_key[ strlen_P(info_table[INFO_KEY]) + 1 ];
    memset( buf_key, 0, strlen_P(info_table[INFO_KEY]) + 1 );
    strcpy_P( buf_key, (char*)pgm_read_word(&info_table[INFO_KEY]) );
    
    char buf_secret[ strlen_P(info_table[INFO_SECRET]) + 1 ];
    memset( buf_secret, 0, strlen_P(info_table[INFO_SECRET]) + 1 );
    strcpy_P( buf_secret, (char*)pgm_read_word(&info_table[INFO_SECRET]) );
  
    char buf_version[ strlen_P(info_table[INFO_VERSION]) + 1 ];
    memset( buf_version, 0, strlen_P(info_table[INFO_VERSION]) + 1 );
    strcpy_P( buf_version, (char*)pgm_read_word(&info_table[INFO_VERSION]) );
  
    Authentication auth( buf_key, buf_secret, buf_version );
  #else
    Authentication auth( CONSUMER_KEY, CONSUMER_SECRET, CLIENT_VERSION );
  #endif

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
  //Serial.println(strlen(password));
  //Serial.println(password);
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
