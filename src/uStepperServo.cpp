/********************************************************************************************
*       File:       uStepperServo.cpp                                                       *
*		Version:    2.3.0                                          						    *
*      	Date: 		December 27th, 2021  	                                    			*
*      	Authors: 	Thomas Hørring Olsen                                   					*
*					Emil Jacobsen															*
*                                                                                           *   
*********************************************************************************************
*                       uStepperServo class                                                 *
*                                                                                           *
*   This file contains the implementation of the class methods, incorporated in the         *
*   uStepperServo Arduino library. Currently the servo pulse are only available on pin 3    *
*   of the uStepper. 
*   
*   The library is used by instantiating an uStepperServo                                   *
*   object, as follows:                                                                     *
*                                                                                           *
*       uStepperServo servo;                                                                *
*                                                                                           * 
*                                                                                           *
*   after instantiation of the object, the object's setup method, should be called within   *
*   arduino's setup function:                                                               *
*                                                                                           *
*       example:                                                                            *
*                                                                                           *
*       uStepperServo servo;                                                                *
*                                                                                           *
*       void setup()                                                                        *
*       {                                                                                   *
*           servo.setup();                                                                  *
*       }                                                                                   *
*                                                                                           *
*   The servo pulse widths are normally around 500 us for 0 deg and 2500 us for 180 deg     *
*   (default values in this library). These values can be redefined to fit your servos      *
*   specifications by calling the setMaximumPulse and SetMinimumPulse functions.            *                         
*                                                                                           *
*       example:                                                                            *
*                                                                                           *
*       uStepperServo servo;                                                                *
*                                                                                           *
*       void setup()                                                                        *
*       {                                                                                   *
*           servo.setup();                                                                  *
*           servo.SetMaximumPulse(2400);                                                    *
*           servo.SetMinimumPulse(600);                                                     *
*       }                                                                                   *
*********************************************************************************************
*   (C) 2021                                                                                *
*                                                                                           *
*   uStepper ApS                                                                            *
*   www.ustepper.com                                                                        *
*   administration@ustepper.com                                                             *
*                                                                                           *
*   The code contained in this file is released under the following open source license:    *
*                                                                                           *
*           Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International         *
*                                                                                           *
*   The code in this file is provided without warranty of any kind - use at own risk!       *
*   neither uStepper ApS nor the author, can be held responsible for any damage             *
*   caused by the use of the code contained in this file !                                  *
*                                                                                           *
********************************************************************************************/
/** @file uStepperServo.cpp
 * @brief      Function prototypes and definitions for the uStepper Servo
 *             library
 *
 *             This file contains the implementations of the classes defined in
 *             uStepperServo.h
 *
 * @author     Thomas Hørring Olsen (thomas@ustepper.com)
 */

#include <uStepperServo.h>

uStepperServo::uStepperServo()
{

}

void uStepperServo::setup(void)
{
    this->setMaximumPulse(2500);
    this->setMinimumPulse(500);
     
    TCCR4A = (1 << 1) | (1 << 5) | (1 << 4); //WGM41 = 1, VGM40 = 0 , Set when up counting, clear when down counting (COM4B0 = 1, COM4B1 = 1)
    TCCR4B = (1 << 4) | (1 << 3); //WGM43 = 1, WGM42 = 1 (Fast PWM mode, TOP = ICR4)
    ICR4 = 39850;
    TIMSK4 = 0; //enable overflow and OCA interrupts
    this->write(0);
    TCCR4B |= (1 << 1); //Enable clock at prescaler 8. 16MHz/8 = 2MHz/40000 = 50Hz Servo Pulse frequency
    DDRD |= (1 << 2);
    PORTD |= (1 << 2);
}

void uStepperServo::setMinimumPulse(float us)
{
    min16 = (uint16_t)(us * TICKSTOUS);
}

void uStepperServo::setMaximumPulse(float us)
{
    max16 = (uint16_t)(us * TICKSTOUS);
}

void uStepperServo::write(int angleArg)
{
    if ( angleArg < 0) angleArg = 0;
    if ( angleArg > 180) angleArg = 180;
    angle = angleArg;
    
    float scale;
    scale = (float)(this->max16 - this->min16);
    scale = scale / 180.0;

    // bleh, have to use longs to prevent overflow, could be tricky if always a 16MHz clock, but not true
    // That 64L on the end is the TCNT0 prescaler, it will need to change if the clock's prescaler changes,
    // but then there will likely be an overflow problem, so it will have to be handled by a human.
    this->pulse = (uint16_t)(scale * (float)angle);
    OCR4B = TIMERTOP - (this->min16 + pulse);
}

