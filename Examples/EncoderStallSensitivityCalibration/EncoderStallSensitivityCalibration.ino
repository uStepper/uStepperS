/********************************************************************************************
* 	    	File:  EncoderStallSensitivityCalibration.ino                                     *
*		   Version:  1.0.0                                                                      *
*         Date:  January 19, 2021                                                           *
*       Author:  Mogens Groth Nicolaisen                                                    *
*  Description:  Encoder Stall Sensitivity Calibration Sketch!                              *
*                This example demonstrates helps calibrate the encoder stall sensitivity    *
*                for use with the encoder stall feature. See encoder stall example.         *
*				         Alternatively the Trinamic Stallguard feature can be used which is shown	  *
*				         in LimitDetection.ino and StallguardIsStalled.ino. Stallguard is very 		  *
*				         sensitive and provides seamless stall detection when tuned for the 		    *
*				         application. It is dependent on speed, current setting and load conditions	*
*				         amongst others. The encoder stall detection is unaffected by most of these *
*				         but can be a bit less sensitive.											                      *
*                                                                                           *
* For more information, check out the documentation:                                        *
*    http://ustepper.com/docs/usteppers/html/index.html                                     *
*                                                                                           *
*                                                                                           *
*********************************************************************************************
*	(C) 2021                                                                                  *
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
*      Encoder Stall Sensitivity Calibration Sketch!
*      
* This example demonstrates helps calibrate the encoder stall sensitivity 
* for use with the encoder stall feature. See encoder stall example.
* For more information, check out the documentation:
* http://ustepper.com/docs/usteppers/html/index.html
*/
#include <uStepperS.h>

uStepperS stepper;
uint8_t rpm[6] = {25, 50, 80, 120, 130, 150};

void setup() {
  // put your setup code here, to run once:
  stepper.setup();
  stepper.checkOrientation(30.0);       //Check orientation of motor connector with +/- 30 microsteps movement
  Serial.begin(9600);
  stepper.encoder.encoderStallDetectEnable = 1; //Enable the encoder stall detect
}

void loop() {
  Serial.println("-- Encoder Stall Test --");

  // Run through all five rpm's
  for( uint8_t i = 0; i < sizeof(rpm); i++ ){
    Serial.print(rpm[i]); Serial.println(" rpm");
    stepper.setRPM(rpm[i]); 
    delay(1000);
    for(uint8_t stallValue = 0; stallValue<220;stallValue++)
    {
      stepper.encoder.encoderStallDetectSensitivity = 1-(stallValue*0.05);
      delay(100);
      if(!stepper.encoder.encoderStallDetect)
      {
        Serial.print("Sensitivity must be less than: ");
        Serial.println(1-(stallValue*0.05));
        stallValue=220;
      }
    }
    stepper.stop();
  } 
}
