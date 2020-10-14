/********************************************************************************************
* 	    	File:  LimitDetection.ino                                                         *
*		   Version:  1.0.0                                                                      *
*         Date:  Oktober 2nd, 2020                                                          *
*       Author:  Emil Jacobsen                                                              *
*  Description:  This example demonstrates how Stallguard can be used to detect motor stall,*
*                and automatic stop before any steps is lost.                               *
*                The for-loop applies six different velocities, but only shifts to the next *
*                when a stall is detected.                                                  *
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

/*  
  Info:   This example demonstrates how Stallguard can be used to detect motor stall, and automatic stop before any steps is lost.
          The for-loop applies six different velocities, but only shifts to the next when a stall is detected.  
          For more information, check out the documentation:
          http://ustepper.com/docs/usteppers/html/index.html
*/             

#include <uStepperS.h>

#define STALLSENSITIVITY 2//sensitivity of the stall detection, between -64 and 63 - high number is less sensitive

uStepperS stepper;
uint8_t rpm[6] = {25, 50, 80, 120, 130, 150};

void setup() {

  Serial.begin(9600);

  // Configure uStepper
  stepper.setup();
  
}


void loop() {

  Serial.println("-- Stallguard test --");

  // Run through all five rpm's
  for( uint8_t i = 0; i < sizeof(rpm); i++ ){
    Serial.print(rpm[i]); Serial.println(" rpm");
    stepper.setRPM(rpm[i]);
    // Enabled stallguard, with threshold set by STALLSENSITIVITY and stop automatic on stall.
    // Hint: If stopOnStall is set to false, you have to use .stop() or setRPM(0) in order to stop the motor on stall. 
    stepper.enableStallguard(STALLSENSITIVITY, true, rpm[i]);

    // Wait for stall to be detected.
    while( !stepper.isStalled() ){}
    // Clear stallguard
    stepper.clearStall();
    
    delay(2000);
  }
  
}