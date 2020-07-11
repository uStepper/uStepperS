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
  stepper.checkOrientation(30.0);       //Check orientation of motor connector with +/- 30 microsteps movement
  Serial.begin(9600);
  stepper.setRPM(100);
}

void loop() {
  char cmd;

  // put your main code here, to run repeatedly:
  while(!Serial.available());
  Serial.println("ACK!");
  cmd = Serial.read();
  if(cmd == '1')                      //Run continous clockwise
  {
    stepper.setRPM(-100);
  }
  
  else if(cmd == '2')                 //Run continous counter clockwise
  {
    stepper.setRPM(100);
  }
  
  else if(cmd == '3')                 //Stop without deceleration and block motor
  {
    stepper.stop();
  }
}