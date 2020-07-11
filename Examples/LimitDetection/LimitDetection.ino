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
#define RES (STEPSPERREV *256)/360.0//calculate step pr. degree
#define STEPPRMM 53.55//step pr. mm for the rail used in the demo
#define MMPRSTEP 1/STEPPRMM//mm pr. step
#define MMPRDEG MMPRSTEP*RES//mm pr. degree
#define STALLSENSITIVITY 2//sensitivity of the stall detection, between -64 and 63 - higher number is less sensitive

uStepperS stepper;

// Desired rpm for homing
int16_t rpm = 50;

void setup() {
  // put your setup code here, to run once:
  stepper.setup(NORMAL, STEPSPERREV); 
  stepper.checkOrientation(30.0);       //Check orientation of motor connector with +/- 30 microsteps movement
  stepper.setMaxAcceleration(2000);//use an acceleration of 2000 fullsteps/s^2
  stepper.setMaxVelocity(500);//Max velocity of 500 fullsteps/s
  Serial.begin(9600);
}

void loop() {
  float railLength;

  stepper.moveToEnd(CW, rpm, STALLSENSITIVITY);      //Reset to CW endpoint
  Serial.println(railLength*MMPRDEG);//find end positions and read out the recorded end position
  railLength = stepper.moveToEnd(CCW, rpm, STALLSENSITIVITY);    //Go to CCW end
  Serial.println(railLength*MMPRDEG);//find end positions and read out the recorded end position
  
  while(1);
}