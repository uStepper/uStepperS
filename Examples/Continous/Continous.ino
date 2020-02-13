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

void setup() {
  // put your setup code here, to run once:
  stepper.setup();

  stepper.driver.enableStallguard();

  Serial.begin(9600);
  stepper.runContinous(CCW);
}

void loop() {
  char cmd;

  bool stall = stepper.getMotorState(STALLGUARD2);

  Serial.println("Stall status: " + String(stall));

  // put your main code here, to run repeatedly:
  while(!Serial.available());
  Serial.println("ACK!");
  cmd = Serial.read();
  if(cmd == '1')                      //Run continous clockwise
  {
    stepper.runContinous(CW);
  }
  
  else if(cmd == '2')                 //Run continous counter clockwise
  {
    stepper.runContinous(CCW);
  }
  
  else if(cmd == '3')                 //Stop without deceleration and block motor
  {
    stepper.stop();
  }
}