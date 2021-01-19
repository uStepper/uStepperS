/********************************************************************************************
*         File:  StallguardSensitivityCalibration.ino                                       *
*      Version:  1.0.0                                                                      *
*         Date:  Januaru 19, 2021                                                           *
*       Author:  Mogens Groth Nicolaisen                                                    *
*  Description:  This example helps tuning Stallguard sensitivity.                          *
*                The for-loop applies six different velocities and finds the sensitivty     *                       
*                that matches for Stallguard.                                               *
*                Stallguard is very sensitive and provides seamless stall detection when    *
*                tuned for the application. It is dependent on speed, current setting       *
*                and load conditions amongst others. The encoder stall detection is         *
*                unaffected by most of these but can be a bit less sensitive.               *
*                                                                                           *
* For more information, check out the documentation:                                        *
*    http://ustepper.com/docs/usteppers/html/index.html                                     *
*                                                                                           *
*                                                                                           *
*********************************************************************************************
* (C) 2021                                                                                  *
*                                                                                           *
* uStepper ApS                                                                              *
* www.ustepper.com                                                                          *
* administration@ustepper.com                                                               *
*                                                                                           *
* The code contained in this file is released under the following open source license:      *
*                                                                                           *
*     Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International               *
*                                                                                           *
*   The code in this file is provided without warranty of any kind - use at own risk!       *
*   neither uStepper ApS nor the author, can be held responsible for any damage             *
*   caused by the use of the code contained in this file !                                  *
*                                                                                           *
*                                                                                           *
********************************************************************************************/

/*
*      Stallguard Sensitivity Calibration Sketch!   
*
* This example helps tuning Stallguard sensitivity.
* For more information, check out the documentation:
* http://ustepper.com/docs/usteppers/html/index.html
*/           

#include <uStepperS.h>
#define STEPSPERREV 200 //This is 200 for a 1.8deg stepper (most common) and e.g. 400 for a 0.9deg stepper

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
    for(int8_t stallValue = -63; stallValue<65;stallValue++)
    {
      stepper.enableStallguard(stallValue, true, rpm[i]);
      delay(200);
      if(stepper.driver.getVelocity()>(rpm[i]-1)*60*256*STEPSPERREV)// Speed in RPM * 60 seconds * 256 usteps * stepsPerRev = steps/s
      {
        Serial.print("Sensitivity ~ ");
        Serial.println(stallValue+1);
        stallValue=65;
      }
    }
    // Clear stallguard
    stepper.stop();
    stepper.clearStall();
    
    delay(2000);
  } 
}
