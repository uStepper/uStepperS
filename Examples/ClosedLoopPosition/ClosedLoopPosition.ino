/*
*      Example sketch for closed loop position control!
*
* This example demonstrates how easy closed loop position control can be achieved using the uStepper S !
* The only thing needed to activate closed loop control, is in the stepper.setup() function, where the
* object is initiated with the keyword "CLOSEDLOOP", followed by the number of steps per revolution setting.
* For more information, check out the documentation:
* http://ustepper.com/docs/usteppers/html/index.html
*
* Once the closed loop is activated, the use of the library is the same as without the closed loop !
*
*/

#include <uStepperS.h>

uStepperS stepper;

void setup(void)
{
  Serial.begin(9600);
  stepper.setup(CLOSEDLOOP,200);     //Initiate the stepper object to use closed loop control with 200 steps per revolution motor - i.e. 1.8 deg stepper 
  
  // For the closed loop position control the acceleration and velocity parameters define the response of the control:
  stepper.setMaxAcceleration(2000);     //use an acceleration of 2000 fullsteps/s^2
  stepper.setMaxVelocity(800);          //Max velocity of 800 fullsteps/s
  
  stepper.checkOrientation(30.0);       //Check orientation of motor connector with +/- 30 microsteps movement
  stepper.setControlThreshold(15);		//Adjust the control threshold - here set to 15 microsteps before making corrective action

  stepper.moveSteps(51200);                 //Turn shaft 51200 steps, counterClockWise (equal to one revolution with the TMC native 1/256 microstepping)
}

void loop(void)
{
  Serial.println(stepper.encoder.getAngleMoved());    //Print out the current angle of the motor shaft.
}