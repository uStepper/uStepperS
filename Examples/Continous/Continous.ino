#include <uStepperS.h>

uStepperS stepper;

void setup() {
  // put your setup code here, to run once:
  stepper.setup();
  Serial.begin(9600);
  stepper.runContinous(CCW);
}

void loop() {
  char cmd;
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