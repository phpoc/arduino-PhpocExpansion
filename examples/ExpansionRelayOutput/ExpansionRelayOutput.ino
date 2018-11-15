// Arduino Example for 4-port Relay Expansion Board (PES-2601)
//
// PES-2601 is an easy-to-use 4-port relay expansion board for Arduino Uno and
// Mega, which allows Arduino to control electric device.
//
// This example demonstrates how Arduino controls and gets state of relays on
// PES-2601.
//
// Arduino communicates with 4-port relay expansion board through PHPoC [WiFi]
// Shield using pins 10, 11, 12 and 13 on the Uno, and pins 10, 50, 51 and 52 on
// the Mega. Therefore, these pins CANNOT be used for general I/O.
//
// This example code was written by Sollae Systems. It is released into the
// public domain.
//
// Tutorial for the example is available here:
// https://forum.phpoc.com/articles/tutorials/1262-arduino-relay-expansion-board

#include <Phpoc.h>
#include <PhpocExpansion.h>

byte expansionId = 1;

ExpansionRelayOutput relay0(expansionId, 0);
ExpansionRelayOutput relay1(expansionId, 1);
ExpansionRelayOutput relay2(expansionId, 2);
ExpansionRelayOutput relay3(expansionId, 3);

void relayPrintState(void) {
  if(relay0.isOn())
    Serial.println("Relay 0 is ON");
  else
    Serial.println("Relay 0 is OFF");

  if(relay1.isOn())
    Serial.println("Relay 1 is ON");
  else
    Serial.println("Relay 1 is OFF");

  if(relay2.isOff())
    Serial.println("Relay 2 is OFF");
  else
    Serial.println("Relay 2 is ON");

  if(relay3.isOff())
    Serial.println("Relay 3 is OFF");
  else
    Serial.println("Relay 3 is ON");

  Serial.println();
}

void setup() {
  Serial.begin(9600);
  while(!Serial)
    ;

  // initialize PHPoC [WiFi] Shield
  Phpoc.begin(PF_LOG_SPI | PF_LOG_NET);
  //Phpoc.begin();

  // initialize expansion board
  Expansion.begin();

  // get name and print it to serial
  Serial.println(relay0.getName());

  // set delay
  //relay0.setDelay(900);
  //relay1.setDelay(1000);
  //relay2.setDelay(1100);
  //relay3.setDelay(1200);
}

void loop() {
  // turn all relays on
  relay0.on();
  relay1.on();
  relay2.on();
  relay3.on();
  delay(1000);

  // get current state and pint to serial
  relayPrintState();

  // turn all relays off
  relay0.off();
  relay1.off();
  relay2.off();
  relay3.off();
  delay(1000);

  // get current state and pint to serial
  relayPrintState();
}
