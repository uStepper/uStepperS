/*
  Name:   StallguardEndstop
  Author: SolidGeek
  
  Info:   This example demonstrates how Stallguard can be used to detect an endstop (homing). 
          The function .moveToEnd() is used to detect an endstop. Could also be done manual using .isStalled().
          For more information, check out the documentation:
          http://ustepper.com/docs/usteppers/html/index.html
*/             

#include <uStepperS.h>

uStepperS stepper;

// Desired rpm for homing
int8_t rpm = 50;

void setup() {

  Serial.begin(9600);

  // Configure uStepper
  stepper.setup();
  
}

void loop() {

  Serial.println("-- MoveToEnd test --");

  // Search for endstop in CW direction, with a stallguard threshold of 4 (can be omitted)
  while( !stepper.moveToEnd(CW, rpm, 4) )
  {
    // Wait for stallguard  
  }

  // Now do stuff

  Serial.println("End reached, press any key to continue");
  
  // Wait for char to be send over Serial
  while( Serial.available() == 0 ){}
  uint8_t c = Serial.read();
  
}