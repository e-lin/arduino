#define ETHERNET_TEST 0

#include <avr/pgmspace.h>
#include <SPI.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <Dns.h>
#include "Authentication_OneFile.h"
#include <sha1.h>
#include <Base64.h>
#include <MemoryFree.h>

const PROGMEM char
  CONSUMER_KEY[] = "nac8c0x8w7om54pz",
  CONSUMER_SECRET[] = "df2ku6pghf5dr4qs346hrnrplbx6w1au",
  CLIENT_VERSION[] = "DefaultStage1";
const char* const info_table[] PROGMEM = { CONSUMER_KEY, CONSUMER_SECRET, CLIENT_VERSION };
#define INFO_KEY 0
#define INFO_SECRET 1
#define INFO_VERSION 2

//#define CONSUMER_KEY "nac8c0x8w7om54pz"
//#define CONSUMER_SECRET "df2ku6pghf5dr4qs346hrnrplbx6w1au"
//#define CLIENT_VERSION "DefaultStage1"

byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0F, 0xF7, 0x7F
};
int ram_space;
char* password;

#if ETHERNET_TEST
EthernetClient client;
#endif

void setup() {
  Serial.begin(115200);

  Serial.println(F("memory(start): "));
  Serial.println( ram_space );
  Serial.println( free_ram() );

#if ETHERNET_TEST
  Serial.print(F("Initializing Ethernet..."));
  if(Ethernet.begin(mac)) {
    Serial.print(F("Connected with IP: "));
    Serial.println(Ethernet.localIP());
  }
#endif

  Serial.println(F("memory: "));
  Serial.println( freeMemory() );
  Serial.println( free_ram() );
  ram_space = freeMemory();
  
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
  //Authentication auth( CONSUMER_KEY, CONSUMER_SECRET, CLIENT_VERSION );
  password = auth.getPassword();
  
  Serial.println(F("memory remain: "));
  Serial.println( freeMemory() );
  Serial.println( free_ram() );
  Serial.println(F("memory usage: "));
  int usage = ram_space - freeMemory();
  Serial.println( usage );
  Serial.println( (ram_space - free_ram()) );

}

void loop() {

  Serial.println(F("memory(loop): "));
  Serial.println( freeMemory() );
  Serial.println( free_ram() );

  Serial.println(strlen(password));
  Serial.println(password);
  
  delay(5000);
}

int free_ram()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

