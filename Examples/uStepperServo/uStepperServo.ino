/********************************************************************************************
* 	    	File:  LimitDetection.ino                                                         *
*		   Version:  1.0.0                                                                      *
*         Date:  Oktober 2nd, 2020                                                          *
*       Author:  Thomas Hørring Olsen                                                       *
*  Description:  uStepper RC Servo Motor Example Sketch!                                    * 
*                This example demonstrates how the library can be used to control an RC     * 
*                servo connected to the uStepper.                                           *
*                                                                                           *
* For more information, check out the documentation:                                        * 
*                http://ustepper.com/docs/usteppers/html/index.html                         *
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

uStepperServo servo;    //instantiate uStepperServo object
uint16_t value;

void setup()
{
  Serial.begin(9600);
  servo.setup();
  servo.setMaximumPulse(2400);    //Set pulsewidth in microSeconds, of 180 degree position
  servo.setMinimumPulse(500);     //Set pulsewidth in microSeconds, of 0 degree position
}

void loop()
{
  if(Serial.available())
  {
    value = Serial.parseInt();      //Read angle argument from serial
    servo.write(value);             //Write angle to servo object
  }
}
