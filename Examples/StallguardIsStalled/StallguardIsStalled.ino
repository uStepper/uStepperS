/*  
  Info:   This example demonstrates how Stallguard can be used to detect motor stall, and automatic stop before any steps is lost.
          The for-loop applies six different velocities, but only shifts to the next when a stall is detected.  
          For more information, check out the documentation:
          http://ustepper.com/docs/usteppers/html/index.html
*/             

#include <uStepperS.h>

#define STALLSENSITIVITY 2//sensitivity of the stall detection, between -64 and 63 - high number is less sensitive

uStepperS stepper;
uint8_t rpm[6] = {25, 50, 80, 120, 130, 150};

void setup() {

  Serial.begin(9600);

  // Configure uStepper
  stepper.setup();
  
}


void loop() {

  Serial.println("-- Stallguard test --");

  // Run through all five rpm's
  for( uint8_t i = 0; i < sizeof(rpm); i++ ){
    Serial.print(rpm[i]); Serial.println(" rpm");
    stepper.setRPM(rpm[i]);
    // Enabled stallguard, with threshold set by STALLSENSITIVITY and stop automatic on stall.
    // Hint: If stopOnStall is set to false, you have to use .stop() or setRPM(0) in order to stop the motor on stall. 
    stepper.enableStallguard(STALLSENSITIVITY, true, rpm[i]);

    // Wait for stall to be detected.
    while( !stepper.isStalled() ){}
    // Clear stallguard
    stepper.clearStall();
    
    delay(2000);
  }
  
}