#define ETHERNET_TEST 1

#include <avr/pgmspace.h>
#include <SPI.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <Dns.h>
#include "Authentication_OneFile.h"
#include <sha1.h>
#include <Base64.h>
#include <MemoryFree.h>

byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0F, 0xF7, 0x7F
};
int ram_space;
//Authentication auth( CONSUMER_KEY, CONSUMER_SECRET, CLIENT_VERSION );
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
  // Initialize Ethernet connection.  Request dynamic
  // IP address, fall back on fixed IP if that fails:
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

  Authentication auth( CONSUMER_KEY, CONSUMER_SECRET, CLIENT_VERSION );
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

