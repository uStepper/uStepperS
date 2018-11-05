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
	 * @brief	Setup function
	 */
	void setup(); 

	void setRPM( int16_t RPM );

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


	void moveSteps( int32_t steps );

	void moveAngle( float angle );

	void moveToAngle( float angle );

	void runContinous( bool direction );

	float angleMoved( void );


private: 

	

	/** This variable contains the maximum velocity, the motor is
	 * allowed to reach at any given point. The user of the library can
	 * set this by use of the setMaxVelocity()
	 */
	float velocity;					

	/** This variable contains the maximum acceleration to be used. The
	 * can be set and read by the user of the library using the
	 * functions setMaxAcceleration()
	 */
	float acceleration;		


	float angleToStep;
	uint16_t microSteps = 256;

	// SPI functions

	uint8_t SPI( uint8_t data );

	void setSPIMode( uint8_t mode );

	void chipSelect( uint8_t pin , bool state );

};



#endif