/********************************************************************************************
*       File:       uStepperServo.h                                                         *
*		    Version:    2.2.0                                           						            *
*      	Date: 		  September 22nd, 2020  	                                    			      *
*      	Authors: 	  Thomas Hørring Olsen                                   					        *
*					          Emil Jacobsen															                              *
*                                                                                           *   
*********************************************************************************************
*   (C) 2020                                                                                *
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
/** @file uStepperServo.h
 * @brief      Function prototypes and definitions for the uStepper Servo
 *             library
 *
 *             This file contains class and function prototypes for the library,
 *             as well as necessary constants and global variables.
 *
 * @author     Thomas Hørring Olsen (thomas@ustepper.com)
 */

#ifndef _USTEPPERSERVO_H
#define _USTEPPERSERVO_H

#include <Arduino.h>
#include <inttypes.h>

#define TIMERTOP 39850.0f
#define TIMERPERIOD 20000.0f //period in us
#define TICKSTOUS TIMERTOP/TIMERPERIOD

/**
 * @brief      Prototype of class for ustepper servo.
 */
class uStepperServo
{
  private:     
    /** Current angle in degrees */
    uint8_t angle;      
    /** Pulse width in timer0 ticks (1 tick = 16us) */
    uint16_t pulse;     
    /** Minimum pulse width in timer0 ticks (default = 92 = 1.472ms) */
    uint16_t min16;      
    /** Maximum pulse width in timer0 ticks (default = 150 = 2.4ms) */
    uint16_t max16;      
  
  public:

    void setup(void);

    /**
     * @brief      Constructor for servo class
     *
     *             This constructor is used to instantiate a new servo class,
     *             and should be used for each servo the user wishes to connect
     *             to the uStepper.
     */
    uStepperServo();
    
    /**
     * @brief      Specify angle of servo motor
     *
     *             This method sets an angle setpoint for the servo motor
     *
     * @param[in]  angleArg  The angle argument
     */
    void write(int angleArg);
    
    /**
     * @brief      Sets the minimum pulse.
     *
     *             This method sets the minimum pulse length given to the servo
     *             motor. This pulse length defines the fully closed angle.
     *
     * @param[in]  t     Minimum pulse width in microseconds
     */
    void setMinimumPulse(float us);
    
    /**
     * @brief      Sets the maximum pulse.
     *
     *             This method sets the maximum pulse length given to the servo
     *             motor. This pulse length defines the fully open angle.
     *
     * @param[in]  t     Maximum pulse width in microseconds
     */
    void setMaximumPulse(float us);     
};

#endif
