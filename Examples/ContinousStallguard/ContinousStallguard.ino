/*
*      Continous Example Sketch!
*
* This example demonstrates how the library can be used to make the motorrun continously,
* in both directions and making it stop an ongoing movement.
* For more information, check out the documentation:
* http://ustepper.com/docs/usteppers/html/index.html
*/

#include <uStepperS.h>

uStepperS stepper;

uint16_t stallValue = 0;

void setup() {

  Serial.begin(9600);

  stepper.setup();

  // Do not use stallguard below 10 RPM, as it is very unstable
  stepper.setRPM(100);

  // Enable stop on stallGuard with threshold 8.
  // Tune to your needs: -64 to +63
  stepper.driver.enableStallguard(true, 5);

  Serial.println("Stallguard ready");

  // Eliminate false positive stallguard on startup
  delay(100);

}

void loop() {

  // Get and print load/stall value to debug
  stallValue = stepper.driver.getStallValue();
  Serial.println( stallValue );

  if( stepper.driver.isStalled() ){

    Serial.println("Motor stalled");
    delay(2000);

    // Clear the stallguard (continue movement)
    stepper.driver.clearStallguard();
  }

}