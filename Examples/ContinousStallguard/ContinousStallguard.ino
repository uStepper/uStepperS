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

bool isStall = true;
uint16_t stallValue = 0;

void setup() {

  Serial.begin(9600);

  stepper.setup();

  // Enable stop on stallGuard with threshold 8.
  // Tune to your needs: -64 to +63
  stepper.driver.enableStallguard(true, 8);

  stepper.runContinous(CW);

}

void loop() {

  isStall = stepper.getMotorState(STALLGUARD2);
  stallValue = stepper.driver.getStallValue();

  Serial.print("Stall value: ");
  Serial.println(stallValue);

  if( isStall == 0 ){

    Serial.println("Motor stalled");
    stepper.driver.clearStallguard();
   
    delay(1000); // !!!!!! TO-DO: Not sure why it doesn't matter to put the delay before or after clearStallguard().
  }

  delay(10);

}
