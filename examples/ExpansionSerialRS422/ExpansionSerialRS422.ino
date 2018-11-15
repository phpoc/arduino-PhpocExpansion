// Arduino Example for RS-422/485 Expansion Board (PES-2607)
//
// PES-2607 is an easy-to-use RS-422/485 expansion board for Arduino Uno and
// Mega, which allows Arduino to exchanging data with serial device via
// RS-422/485.
//
// This is an Arduino example that receive data from a serial device, send back
// the serial device via RS-422 and print the data to serial monitor.
//
// Arduino communicates with RS-422/485 expansion board through PHPoC [WiFi]
// Shield using pins 10, 11, 12 and 13 on the Uno, and pins 10, 50, 51 and 52 on
// the Mega. Therefore, these pins CANNOT be used for general I/O.
//
// This example code was written by Sollae Systems. It is released into the
// public domain.
//
// Tutorial for the example is available here:
// https://forum.phpoc.com/articles/tutorials/1267-arduino-rs-422-expansion-board

#include <Phpoc.h>
#include <PhpocExpansion.h>

// size of application buffer, reduce it if memory of Arduino is not enough
#define BUFFER_SIZE 100

byte expansionId = 1;

ExpansionSerial rs422(expansionId);

// application buffer
byte rwbuf[BUFFER_SIZE];

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
  Serial.println(rs422.getName());

  // set baudrate, parity, data bit, stop bit and flow control to  115200, none,
  // 8 bit, 1 bit and no flow control, respectively.
  rs422.begin(F("115200N81N"));
}

void loop() {
  // get the size of received data in the receiving buffer of RS-422/485
  // expansion board
  int rxlen = rs422.available();

  if(rxlen > 0) {
    // read and write length
    int rwlen;

    // get the size of free space in the sending buffer of RS-422/485 expansion
    // board
    int txfree = rs422.availableForWrite();

    // check to make sure that read data does not exceed size of application
    // buffer
    rwlen = min(rxlen, BUFFER_SIZE);

    // check to make sure that write data does not exceed free space in the
    // sending buffer of RS-422/485 expansion board
    rwlen = min(rwlen, txfree);

    // receive data from the receiving buffer of RS-422/485 expansion board and
    // save it to the application buffer
    rwlen = rs422.readBytes(rwbuf, rwlen);

    // send data from the application buffer to the sending buffer of RS-422/485
    // expansion board
    rs422.write(rwbuf, rwlen);

    // print data to serial monitor of Arduino IDE
    Serial.write(rwbuf, rwlen);
  }
}
