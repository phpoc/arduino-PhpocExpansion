// Arduino Example for Brushed DC Motor Controller (PES-2604)
//
// PES-2604 is an easy-to-use DC motor controller for Arduino Uno and Mega.
//
// This example demonstrates how Arduino controls DC motor and gets position and
// speed feedback from encoder using PES-2604.
//
// Arduino communicates with DC motor controller through PHPoC [WiFi] Shield
// using pins 10, 11, 12 and 13 on the Uno, and pins 10, 50, 51 and 52 on the
// Mega. Therefore, these pins CANNOT be used for general I/O.
//
// This example code was written by Sollae Systems. It is released into the
// public domain.
//
// Tutorial for the example is available here:
// https://forum.phpoc.com/articles/tutorials/1264-arduino-dc-motor-controller

#include <Phpoc.h>
#include <PhpocExpansion.h>

byte expansionId = 1;

ExpansionDCMotor dcm1(expansionId, 1);
//ExpansionDCMotor dcm2(expansionId, 2);

int width = 10000;

void setup() {
  Serial.begin(9600);
  while(!Serial)
    ;

  // initialize PHPoC [WiFi] Shield
  Phpoc.begin(PF_LOG_SPI | PF_LOG_NET);
  //Phpoc.begin();

  // initialize dc motor controller
  Expansion.begin();

  // get name and print it to serial
  Serial.println(dcm1.getName());

  // for advanced usage
  //dcm1.setPolarity(-1);
  //dcm1.setDirection(-1);
  //dcm1.setDecay(1);
  //dcm1.setEncoderPolarity(-1);
  //dcm1.setEncoderPosition(100000);
  //dcm1.setEncoderPSR(32);
  //dcm1.setFilterFrequency(5000);
  //dcm1.setFilterPNC(0);

  // set PWM period
  dcm1.setPeriod(10000);
}

void loop() {
  // set duration of the high level in a PWM cycle
  dcm1.setWidth(width);
  delay(1000);

  // get encoder position, unit is pulse
  long pos = dcm1.getEncoderPosition();
  // get encoder period, unit is microsecond per pulse
  long period = dcm1.getEncoderPeriod();
  // calculate motor speed, unit is pulse per second
  long speed = 1000000 / period;

  // print to serial
  Serial.print(F("position: "));
  Serial.print(pos);
  Serial.print(F(" / "));
  Serial.print(F("period: "));
  Serial.print(period);
  Serial.print(F(" / "));
  Serial.print(F("speed: "));
  Serial.println(speed);

  // slow down motor by decreasing the duration of the high level.
  if(width > 0)
    width -= 1000;
}
