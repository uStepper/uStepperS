/********************************************************************************************
* 	    	File:  VelocityTest.ino                                                           *
*		Version:    2.3.0                                          						    *
*      	Date: 		December 27th, 2021  	                                    			*
*       Author:  Hans Henrik Skovgaard                                                      *
*  Description:  By entering the commands f, m and s the RPM of the stepper can be changed  *
*                 on the fly.                                                               *
*  Commands...:  f: fast, set rpm = 1500,                                                   *
*                m: medium, set rpm = 1000,                                                 *
*                s: slow, set rpm = 500,                                                    *
*                h: halt                                                                    *
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

#define MAXACCELERATION 1500  //Max acceleration = 1500 Steps/s^2
#define MAXVELOCITY 1500      //Max velocity = 1500 steps/s

uStepperS stepper(MAXACCELERATION, MAXVELOCITY);

void setup() 
{
  // put your setup code here, to run once:
  stepper.setup();
  Serial.begin(9600);
  Serial.println("Speed test. Ver: 2020-10-02.\n Commands: f:fast, m:medium, s:slow, h:halt");  
  stepper.runContinous(CCW);
}

void loop() 
{
  char cmd;
  // put your main code here, to run repeatedly:
  while(!Serial.available());
  Serial.println("ACK!");
  cmd = Serial.read();
  if( (cmd == 'f') || (cmd == 'F') ) //Run fast
  {
    stepper.setMaxVelocity(1500);
  }
  else if( (cmd == 'm') || (cmd == 'M') ) //Run medium
  {
    stepper.setMaxVelocity(1000);
  }
  else if( (cmd == 's') || (cmd == 'S') ) // Run slow
  {
    stepper.setMaxVelocity(500);
  }
  else if( (cmd == 'h') || (cmd == 'H') ) // Halt
  {
    stepper.stop( );
  }
}
