// Arduino Example for Stepper Motor Controller (PES-2605)
//
// PES-2605 is an easy-to-use stepper motor controller for Arduino Uno and Mega,
// which uses micro-stepping method to precisely control stepper motor.
//
// This example demonstrates how Arduino controls step motor to specific
// positions at specific speed, acceleration and deceleration using PES-2605.
//
// Arduino communicates with stepper motor controller through PHPoC [WiFi]
// Shield using pins 10, 11, 12 and 13 on the Uno, and pins 10, 50, 51 and 52 on
// the Mega. Therefore, these pins CANNOT be used for general I/O.
//
// This example code was written by Sollae Systems. It is released into the
// public domain.
//
// Tutorial for the example is available here:
// https://forum.phpoc.com/articles/tutorials/1265-arduino-stepper-motor-controller

#include <Phpoc.h>
#include <PhpocExpansion.h>

byte expansionId = 1;
int loopCount = 5;

ExpansionStepper step(expansionId);

void setup() {
  Serial.begin(9600);
  while(!Serial)
    ;

  // initialize PHPoC [WiFi] Shield
  Phpoc.begin(PF_LOG_SPI | PF_LOG_NET);
  //Phpoc.begin();

  // initialize stepper motor controller
  Expansion.begin();

  // get name and print it to serial
  Serial.println(step.getName());

  // for advanced usage
  //step.setVrefStop(2);
  //step.setVrefDrive(8);
  //step.setVrefLock(0);
  //step.setResonance(120, 250);
  //step.setPosition(2000);

  // set microstep resolution to 1/32
  step.setMode(32);
  // set speed to 20000
  step.setSpeed(20000);
  // set acceleration and deceleration to 50000 and 50000, respectively
  step.setAccel(50000, 50000);
}

void loop() {
  if(!loopCount)
    return;
  else
    loopCount--;

  // make motor move 10000 steps in a direction. This function is blocking
  step.stepMove(10000);
  // get current step position and print it to serial
  Serial.println(step.getPosition());

  // make motor move 5000 steps in a reverse direction. This function is
  // blocking
  step.stepMove(-5000);
  // get current step position and print it to serial
  Serial.println(step.getPosition());

  // make motor move to a position that is 10000 steps away from initial
  // position in a direction. This function is non-blocking
  step.stepGoto(10000);
  // wait until motor stops
  while(step.getState())
    ;
  // get current step position and print it to serial
  Serial.println(step.getPosition());

  // make motor move to a position that is 10000 steps away from initial
  // position in a reverse direction. This function is non-blocking
  step.stepGoto(-10000);
  // wait until motor stops
  while(step.getState())
    ;
  // get current step position and print it to serial
  Serial.println(step.getPosition());
}
