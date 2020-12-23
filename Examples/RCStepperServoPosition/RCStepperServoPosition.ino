/********************************************************************************************
*         File:  RCStepperServoPosition.ino                                                 *
*      Version:  1.0.0                                                                      *
*         Date:  December 23rd, 2020                                                        *
*       Author:  Mogens Groth Nicolaisen                                                    *
*  Description:  Example sketch for control of stepper using an RC radio!                   *         
*                This example demonstrates the use of uStepper S as an RC servo             *
*                taking input from an RC receiver and performing closed loop motion.        *
*                Set the desired max and min position, acceleration and speed.              *
*                The example requires the user to give a low signal (hold the transmitter   * 
*                stick low for 1 second followed by moving it to the highest positon        *
*                for a second to calibrate the movement range.                              *
*                                                                                           *
*  Pin connections:                                                                         *
* -----------------------------                                                             *
* | Controller | uStepper S    |                                                            *
* |----------------------------|                                                            *
* | Signal   |    D3     |                                                                  *
* | GND      |    GND    |                                                                  *
* | 5V       |    5V     | <- This is only neded if you do not have a supply for your       *
*                             receiver - ONLY add one 5V connection to your receiver !!     *
* ------------------------------                                                            *
*                                                                                           *
*                                                                                           *
* For more information, check out the documentation:                                        *
*    http://ustepper.com/docs/usteppers/html/index.html                                     *
*                                                                                           *
*                                                                                           *
*********************************************************************************************
* (C) 2020                                                                                  *
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

#include <uStepperS.h>

#define MINPOS -90
#define MAXPOS 90

uStepperS stepper;
uint16_t deltaTime = 0;
uint16_t time1 = 0;
uint16_t lowVal = 2000;
uint16_t highVal = 0;
uint8_t k = 1;
int16_t posFilt;
bool RCinit = 0;

void setup(void)
{
  stepper.setup(CLOSEDLOOP,200,1,1,1,1,0);    //Initiate the stepper object to use closed loop control with 200 steps per revolution motor - i.e. 1.8 deg stepper and 0 at the end to use encoder absolute zero as reference
  pinMode(3,INPUT_PULLUP);    //Hardware interrupt input on D3 with pull-up
  attachInterrupt(0, interruptRC, CHANGE);    //Attach interrupt to the pin and react on both rising and falling edges
  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);
  
  // For the closed loop position control the acceleration and velocity parameters define the response of the control:
  stepper.setMaxAcceleration(50000);     //use an acceleration of 2000 fullsteps/s^2
  stepper.setMaxVelocity(12000);          //Max velocity of 800 fullsteps/s
  stepper.checkOrientation(30.0);       //Check orientation of motor connector with +/- 30 microsteps movement
  stepper.setControlThreshold(30);		//Adjust the control threshold - here set to 15 microsteps before making corrective action
}

void loop(void)
{
  while(!RCinit)// Calibrate max/min of RC input before doing anything else
  {
    initRC();
  }
  int16_t servoPos = map(deltaTime, lowVal, highVal, MINPOS, MAXPOS);
  posFilt = (posFilt<< 2)-posFilt;//  Filtering a bit on the signal to make it nice
  posFilt += servoPos;
  posFilt >>= 2;
  stepper.moveToAngle(posFilt);// Now move as requested
}

void initRC(void)// Find max and min limits of control input. Requires that the user moves the stick in one direction and keeps it there for a second and then move in the opposite direction and keep it there for a second
{
  if(deltaTime < 1200 && k<50)
  {
    if(deltaTime<lowVal)
    {
      lowVal = deltaTime;
    }
    k++;
    digitalWrite(2,LOW);
    delay(20);
  }
  else if(deltaTime > 1700 && k>49 && k<100)
  {
    if(deltaTime>highVal)
    {
      highVal = deltaTime;
    }
    k++;
    digitalWrite(2,LOW);
    delay(20);
  }
  else
  {
    digitalWrite(2,HIGH);
  }
  if(k==100 && deltaTime > ((highVal+lowVal)/2)-10 && deltaTime < ((highVal+lowVal)/2)+10)
  {
    RCinit = 1;
  }
}

void interruptRC(void)
{
  if(PIND & (1<<PD2))// Check if input is high
  {
    time1 = micros();
  }
  else//Now it is low - calc time
  {
    deltaTime = micros()-time1;
  }
}
