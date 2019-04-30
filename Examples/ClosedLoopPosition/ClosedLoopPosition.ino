/*
*      Example sketch for closed loop position control!
*
* This example demonstrates how easy closed loop position control can be achieved using the uStepper S !
* The only thing needed to activate closed loop control, is in the stepper.setup() function, where the
* object is initiated with the keyword "PID", followed by the number of steps per revolution setting.
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
  stepper.setup(PID,200);     //Initiate the stepper object to use closed loop control
                                                //note that the closed loop control are NOT using a PID controller
                                                //Which is why P, I and D are not configured
                                                //check out the documentation:
                                                //http://ustepper.com/docs/usteppers/html/index.html

  stepper.moveSteps(3200);                 //turn shaft 3200 steps, counterClockWise (equal to one revolution)
}

void loop(void)
{
  Serial.println(stepper.encoder.getAngleMoved());    //print out the current angle of the motor shaft.
}