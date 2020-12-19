/********************************************************************************************
* 	    	File:  encoderStall.ino                                                           *
*		   Version:  1.0.0                                                                      *
*         Date:  Oktober 7th, 2020                                                          *
*       Author:  Mogens Groth Nicolaisen                                                    *
*  Description:  Encoder Stall Example Sketch!                                              *
*                This example demonstrates how the library can be used to detect a stall    *
*                using the encoder feedback and stop the motor.                             *
*                                                                                           *
* For more information, check out the documentation:                                        *
*    http://ustepper.com/docs/usteppers/html/index.html                                     *
*                                                                                           *
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

/*
*      Encoder Stall Example Sketch!
*
* This example demonstrates how the library can be used to detect a stall using the encoder 
* feedback and stop the motor.
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
  stepper.encoder.encoderStallDetectSensitivity = -0.5;//Encoder stalldetect sensitivity - From -10 to 1 where lower number is less sensitive and higher is more sensitive. -0.5 works for most.
  stepper.encoder.encoderStallDetectEnable = 1; //Enable the encoder stall detect
}

void loop() {
  bool stall = stepper.encoder.encoderStallDetect;//Read the stall decision
  Serial.println(stall);// Print out the result - 1 = stall detected
  if(stall)// Look for stall
    {
      stepper.stop();// Stop motor !
      while(1);// Stop program
    }
}
