/*
*      Limit Detection Example Sketch!
*
* This example demonstrates how the library can be used to detect hard limits in an application,
* without the use of mechanical endstop switches.
* For more information, check out the documentation:
* http://ustepper.com/docs/usteppers/html/index.html
*
*/
#include <uStepperS.h>

#define STEPSPERREV 200 //Number of steps pr revolution. 200 for a 1.8deg motor, 400 for a 0.9deg motor
#define RES (200.0*256)/360.0//calculate step pr. degree
#define STEPPRMM 53.55//step pr. mm for the rail used in the demo
#define MMPRSTEP 1/STEPPRMM//mm pr. step
#define MMPRDEG MMPRSTEP*RES//mm pr. degree
#define STALLSENSITIVITY 0.9//sensitivity of the stall detection, between 0.0 and 1.0 - high number is less sensitive

uStepperS stepper;

void setup() {
  // put your setup code here, to run once:
  stepper.setup();
  stepper.setMaxAcceleration(2000);//use an acceleration of 2000 fullsteps/s^2
  stepper.setMaxVelocity(500);//Max velocity of 500 fullsteps/s
  Serial.begin(9600);
}

void loop() {
  float railLength;

  stepper.moveToEnd(CW, STALLSENSITIVITY);      //Reset to CW endpoint
  Serial.println(railLength*MMPRDEG);//find end positions and read out the recorded end position
  railLength = stepper.moveToEnd(CCW, STALLSENSITIVITY);    //Go to CCW end
  Serial.println(railLength*MMPRDEG);//find end positions and read out the recorded end position
  
  while(1);
}