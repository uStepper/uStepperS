/********************************************************************************************
* 	    	File:  LimitDetection.ino                                                         *
*		Version:    2.3.0                                          						    *
*      	Date: 		December 27th, 2021  	                                    			*
*       Author:  Thomas Hørring Olsen                                                       *
*  Description:  Limit Detection Example Sketch!                                            *
*                This example demonstrates how the library can be used to detect hard       *
*                limits in an application, without the use of mechanical endstop switches.  *
*                Stallguard is very sensitive and provides seamless stall detection when 	  *
*				         tuned for the application. It is dependent on speed, current setting		    *
*				         and load conditions amongst others. The encoder stall detection is 		    *
*				         unaffected by most of these but can be a bit less sensitive.				        *
*                                                                                           *
* For more information, check out the documentation:                                        *
*                       http://ustepper.com/docs/usteppers/html/index.html                  *
*                                                                                           *
*  Note.......:  Place the stepper motor on a solid surface where it cannot harm anyone     *
*                during the test.                                                           *
*                                                                                           *
*********************************************************************************************
*	(C) 2020                                                                                  *
*                                                                                           *
*	uStepper ApS                                                                              *
*	www.ustepper.com                                                                          *
*	administration@ustepper.com                                                               *
*                                                                                           *
*	The code contained in this file is released under the following open source license:      *
*                                                                                           *
*			Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International               *
*                                                                                           *
* 	The code in this file is provided without warranty of any kind - use at own risk!       *
* 	neither uStepper ApS nor the author, can be held responsible for any damage             *
* 	caused by the use of the code contained in this file !                                  *
*                                                                                           *
*                                                                                           *
********************************************************************************************/

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
