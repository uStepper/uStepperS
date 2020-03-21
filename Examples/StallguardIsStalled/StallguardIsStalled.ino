/*  
  Info:   This example demonstrates how Stallguard can be used to detect motor stall, and automatic stop before any steps is lost.
          The for-loop applies six different velocities, but only shifts to the next when a stall is detected.  
          For more information, check out the documentation:
          http://ustepper.com/docs/usteppers/html/index.html
*/             

#include <uStepperS.h>

uStepperS stepper;
uint8_t rpm[6] = {50, 60, 70, 80, 90, 100};

void setup() {

  Serial.begin(9600);

  // Configure uStepper
  stepper.setup();

  // Enabled stallguard, with threshold 4 and stop automatic on stall.
  // Hint: If stopOnStall is set to false, you have to use .stop() or setRPM(0) in order to stop the motor on stall. 
  stepper.enableStallguard(4, true);
  
}


void loop() {

  Serial.println("-- Stallguard test --");

  // Run through all five rpm's
  for( uint8_t i = 0; i < sizeof(rpm); i++ ){
    Serial.print(rpm[i]); Serial.println(" rpm");
    stepper.setRPM(rpm[i]);

    // Wait for stall to be detected.
    while( !stepper.isStalled() ){}
    // Clear stallguard
    stepper.clearStall();
    
    delay(2000);
  }
  
}