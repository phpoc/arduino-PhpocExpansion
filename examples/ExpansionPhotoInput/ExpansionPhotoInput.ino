// Arduino Example for 4-Port Digital Input Expansion Board (PES-2602)
//
// PES-2602 is an easy-to-use 4-port digital input expansion board for Arduino
// Uno and Mega, which allows Arduino to monitor state of electric device.
//
// This example demonstrates how Arduino monitors state of electric device,
// which connects to PES-2602.
//
// Arduino communicates with 4-port digital input expansion board through PHPoC
// [WiFi] Shield using pins 10, 11, 12 and 13 on the Uno, and pins 10, 50, 51
// and 52 on the Mega. Therefore, these pins CANNOT be used for general I/O.
//
// This example code was written by Sollae Systems. It is released into the
// public domain.
//
// Tutorial for the example is available here:
// https://forum.phpoc.com/articles/tutorials/1263-arduino-digital-input-board

#include <Phpoc.h>
#include <PhpocExpansion.h>

byte expansionId = 1;

ExpansionPhotoInput in0(expansionId, 0);
ExpansionPhotoInput in1(expansionId, 1);
ExpansionPhotoInput in2(expansionId, 2);
ExpansionPhotoInput in3(expansionId, 3);

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
  Serial.println(in0.getName());

  // set the debounce time
  //in0.setDelay(900);
  //in1.setDelay(1000);
  //in2.setDelay(1100);
  //in3.setDelay(1200);
}

void loop() {
  // get state of each input port and print to serial
  if(in0.isOn())
    Serial.println("Input port 0 is ON");
  else
    Serial.println("Input port 0 is OFF");

  if(in1.isOn())
    Serial.println("Input port 1 is ON");
  else
    Serial.println("Input port 1 is OFF");

  if(in2.isOff())
    Serial.println("Input port 2 is OFF");
  else
    Serial.println("Input port 2 is ON");

  if(in3.isOff())
    Serial.println("Input port 3 is OFF");
  else
    Serial.println("Input port 3 is ON");

  Serial.println();

  delay(1000);
}
