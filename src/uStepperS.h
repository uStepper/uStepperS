/********************************************************************************************
* 	 	File: 		uStepperS.h 																*
*		Version:    1.0.0                                         						*
*      	date: 		July 12th, 2018 	                                    				*
*      	Author: 	Emil Jacobsen 		                                  					*
*                                                   										*	
*********************************************************************************************
*	(C) 2018																				*
*																							*
*	uStepper ApS																			*
*	www.ustepper.com 																		*
*	administration@ustepper.com 															*
*																							*
*	The code contained in this file is released under the following open source license:	*
*																							*
*			Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International			*
* 																							*
* 	The code in this file is provided without warranty of any kind - use at own risk!		*
* 	neither uStepper ApS nor the author, can be held responsible for any damage				*
* 	caused by the use of the code contained in this file ! 									*
*                                                                                           *
********************************************************************************************/

/**
 * @file uStepperS.h
 * @brief	Function prototypes and definitions for the uStepperS library
 *
 *			This file contains class and function prototypes for the library,
 *			as well as necessary constants and global variables.
 *
 * @author	Emil Jacobsen (emil@ustepper.com)
 */

#ifndef _USTEPPER_S_H_
#define _USTEPPER_S_H_

#ifndef __AVR_ATmega328PB__
	#error !!This library only supports the ATmega328PB MCU!!
#endif



#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>
class uStepperS;
#include <uStepperEncoder.h>
#include <uStepperDriver.h>


#define DRV_ENN PD4 
#define SD_MODE PD5

#define CS_DRIVER PE2
#define CS_ENCODER PD7 

#define MOSI1 PE3
#define MOSI_ENC PC2
#define MISO1 PC0  
#define SCK1 PC1 

#define CLOCKFREQ 16000000.0
#define FREQSCALE 16777216.0 / CLOCKFREQ

/** Frequency at which the encoder is sampled, for keeping track of angle moved and current speed */
#define ENCODERINTFREQ 1000.0			
/** Constant to convert angle difference between two interrupts to speed in revolutions per second. Dividing by 10 as each speed is calculated from 10 samples */
#define ENCODERSPEEDCONSTANT ENCODERINTFREQ/10.0/65535.0	


/**
 * @brief	Measures angle of motor.
 *
 *			This interrupt routine is in charge of sampling the encoder
 */
extern "C" void TIMER1_COMPA_vect(void) __attribute__ ((signal,used));

class uStepperS
{

friend class uStepperDriver;
friend class uStepperEncoder;

public:			
	/** Instantiate object for the encoder */

    // uStepperEncoder *encoder;

	/** Instantiate object for the driver */
	uStepperDriver driver;
	
	/** Instantiate object for the driver */
	uStepperEncoder encoder;

	/**
	 * @brief	Constructor of uStepper class
	 */
	uStepperS();

	uStepperS(float acceleration, float velocity);

	/**
	 * @brief	Internal function to prepare the uStepperS in the constructor
	 */
	void init( void );

	/**
	 * @brief	Setup function
	 */
	void setup(); 


	/**
	 * @brief      Set the velocity in rpm
	 *
	 *             This function lets the user set the velocity of the motor in rpm. 
	 *             A negative value switches direction of the motor.
	 *
	 * @param      rpm  - The velocity in rotations per minute
	 */
	void setRPM( float rpm );

	/**
	 * @brief      Set the maximum acceleration of the stepper motor.
	 *
	 *             This function lets the user set the max acceleration used 
	 *             by the stepper driver.
	 *
	 * @param      acceleration  - Maximum acceleration in steps/s^2
	 */
	void setMaxAcceleration	( float acceleration );

	/**
	 * @brief      Set the maximum deceleration of the stepper motor.
	 *
	 *             This function lets the user set the max deceleration used 
	 *             by the stepper driver.
	 *
	 * @param      deceleration  - Maximum deceleration in steps/s^2
	 */
	void setMaxDeceleration ( float deceleration );

	/**
	 * @brief      Set the maximum velocity of the stepper motor.
	 *
	 *             This function lets the user set the max velocity used 
	 *             by the stepper driver.
	 *
	 * @param      velocity  - Maximum velocity in steps/s^2
	 */
	void setMaxVelocity	( float velocity );

	/**
	 * @brief      Set motor output current.
	 *
	 *             This function allows the user to change the current setting of the motor driver.
	 *
	 * @param      current  - Desired current in percent (0% - 100%)
	 */
	void setCurrent( double current );

	/**
	 * @brief      Set motor hold current.
	 *
	 *             This function allows the user to change the current setting of the motor driver.
	 *
	 * @param      current  - Desired hold current in percent (0% - 100%)
	 */
	void setHoldCurrent( double current );

	/**
	 * @brief      Make the motor perform a predefined number of steps
	 *
	 *             This function makes the motor perform a predefined number of
	 *             steps, using the acceleration profile. The motor will accelerate
	 *             at the rate set by setMaximumAcceleration(), and eventually reach the speed set
	 *             by setMaximumVelocity() function. The direction of rotation
	 *             is set by the argument "dir".
	 *
	 * @param      steps     -	Number of steps to be performed.
	 */
	void moveSteps( int32_t steps );

	/**
	 * @brief      	Moves the motor to a relative angle
	 *
	 * @param  	    angle     -	Relative angle from current position (negative value allowed)
	 */
	void moveAngle( float angle );

	/**
	 * @brief      	Moves the motor to an absolute angle
	 *
	 * @param   	angle     -	Absolute angle (negative value allowed)
	 */
	void moveToAngle( float angle );

	/**
	 * @brief      Make the motor rotate continuously
	 *
	 *             This function makes the motor rotate continuously, using the
	 *             acceleration profile.
	 *
	 * @param      dir   - Can be set to "CCW" or "CW" (without the quotes)
	 */
	void runContinous( bool dir );

	/**
	 * @brief      Get the angle moved from reference position
	 *
	 * @return     The angle moved.
	 */
	float angleMoved( void );

	void stop( void );

private: 

	/** This variable contains the maximum velocity in steps/s, the motor is
	 * allowed to reach at any given point. The user of the library can
	 * set this by use of the setMaxVelocity()
	 */
	float maxVelocity;					

	/** This variable contains the maximum acceleration in steps/s to be used. The
	 * can be set and read by the user of the library using the
	 * functions setMaxAcceleration()
	 */
	float maxAcceleration;
	float maxDeceleration;

	float rpmToVelocity;
	float angleToStep;

	uint16_t microSteps = 256;
	uint16_t fullSteps = 200;

	// SPI functions

	uint8_t SPI( uint8_t data );

	void setSPIMode( uint8_t mode );

	void chipSelect( uint8_t pin , bool state );

};



#endif