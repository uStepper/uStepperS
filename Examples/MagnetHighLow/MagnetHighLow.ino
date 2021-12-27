/********************************************************************************************
* 	    	File:  MHI_MLOTest.ino                                                            *
*		Version:    2.3.0                                          						    *
*      	Date: 		December 27th, 2021  	                                    			*
*       Author:  Hans Henrik Skovgaard                                                      *
*  Description:  This example displays the status of the magnet: High or low, meaning too   *
*                    close or too far away. Both situations can cource reading errors.      *
*                                                                                           *
*  Commands...:  h/H: halt the stepper motor.                                               *
*                s/S: start the stepper motor.                                              *
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

#define MAXACCELERATION 1500         //Max acceleration = 1500 Steps/s^2
#define MAXVELOCITY 50           //Max velocity = 1100 steps/s

uStepperS stepper(MAXACCELERATION, MAXVELOCITY);

void setup() {
  // put your setup code here, to run once:
  stepper.setup();
  Serial.begin(9600);
  Serial.println("Display magnet status.  Ver: 2020-10-02\n Commands: h:halt, s:start");  
  stepper.runContinous(CCW);
}

void loop() {
  char cmd;
  uint8_t encoderStatus;

  if ( Serial.available() )
  {
    Serial.println("ACK!");
    cmd = Serial.read();
    if( (cmd == 'h') || (cmd == 'H') ) // Halt
    {
      stepper.stop( );
    }
    if( (cmd == 's') || (cmd == 'S') ) // start
    {
      stepper.runContinous(CCW);
    }    
    //while(!Serial.available());
  }
  encoderStatus = stepper.encoder.getStatus();
  Serial.print("encoder status: ");    Serial.println(encoderStatus, BIN);  
   
  // MHI: &0x40
  if( encoderStatus & 0x40 )
  {
     Serial.println("Magnet strength too high.");
  }
  // MLO: &0x20
  if( encoderStatus & 0x20 )
  {
     Serial.println("    Magnet strength too low.");
  }  
  // Ready: 0x80
  if( encoderStatus & 0x80 )
  {
     Serial.println("      The chip is ready.");
  }  
  // Even: parity 0x10
  if( encoderStatus & 0x10 )
  {
     Serial.println("Even: parity.");
  }
     encoderStatus = 0;  
  delay(1000);
}
