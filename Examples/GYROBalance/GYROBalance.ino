/*
*      Gyro Example Sketch!
*      Requires uStepper S Gyro Shield !
*      
* This example demonstrates the use of a gyro/accelerometer for feedback to the uStepper.
*
* The MPU6050 src files are derivatives from the TinyMPU6050 library - licensing info is found in "LICENSE_TinyMPU6050" 
*
* For more information, check out the documentation:
* http://ustepper.com/docs/usteppers/html/index.html
*/

#include <uStepperS.h>
uStepperS stepper;
#include "TinyMPU6050.h"

MPU6050 mpu;

void setup(){
  stepper.setup();        //Initialisation of the uStepper S
  stepper.checkOrientation(10.0);       //Check orientation of motor connector
  stepper.setup(CLOSEDLOOP,200);     //Initiate the stepper object to use closed loop control
  stepper.setMaxAcceleration(500);     //use an acceleration of 500 fullsteps/s^2
  stepper.setMaxVelocity(500);          //Max velocity of 500 fullsteps/s
  stepper.setControlThreshold(10);
   // Initialization
  mpu.Initialize();
  // Calibration - LED will blink until calibration is done !
  mpu.Calibrate();
  stepper.encoder.setHome(-mpu.GetAngZ());
}

void loop(){
  mpu.Execute();
  stepper.moveToAngle(-mpu.GetAngZ());
}