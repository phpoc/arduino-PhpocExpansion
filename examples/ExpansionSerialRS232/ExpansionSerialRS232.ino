// Arduino Example for RS-232 Expansion Board (PES-2606)
//
// PES-2606 is an easy-to-use RS-232 expansion board for Arduino Uno and Mega,
// which allows Arduino to exchanging data with serial device via RS-232.
//
// This is an Arduino example that receive data from a serial device, send back
// the serial device via RS-232 and print the data to serial monitor.
//
// Arduino communicates with RS-232 expansion board through PHPoC [WiFi] Shield
// using pins 10, 11, 12 and 13 on the Uno, and pins 10, 50, 51 and 52 on the
// Mega. Therefore, these pins CANNOT be used for general I/O.
//
// This example code was written by Sollae Systems. It is released into the
// public domain.
//
// Tutorial for the example is available here:
// https://forum.phpoc.com/articles/tutorials/1266-arduino-rs-232-expansion-board

#include <Phpoc.h>
#include <PhpocExpansion.h>

// size of application buffer, reduce it if memory of Arduino is not enough
#define BUFFER_SIZE 100

byte expansionId = 1;

ExpansionSerial rs232(expansionId);

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
  Serial.println(rs232.getName());

  // set baudrate, parity, data bit, stop bit and flow control to  115200, none,
  // 8 bit, 1 bit and no flow control, respectively.
  rs232.begin(F("115200N81N"));
}

void loop() {
  // get the size of the received data in the receiving buffer of RS-232
  // expansion board
  int rxlen = rs232.available();

  if(rxlen > 0) {
    // read and write length
    int rwlen;

    // get the size of free space in the sending buffer of RS-232 expansion
    // board
    int txfree = rs232.availableForWrite();

    // check to make sure that read data does not exceed size of application
    // buffer
    rwlen = min(rxlen, BUFFER_SIZE);

    // check to make sure that write data does not exceed free space in the
    // sending buffer of RS-232 expansion board
    rwlen = min(rwlen, txfree);

    // receive data from the receiving buffer of RS-232 expansion board and save
    // it to the application buffer
    rwlen = rs232.readBytes(rwbuf, rwlen);

    // send data from the application buffer to the sending buffer of RS-232
    // expansion board
    rs232.write(rwbuf, rwlen);

    // print data to serial monitor of Arduino IDE
    Serial.write(rwbuf, rwlen);
  }
}
