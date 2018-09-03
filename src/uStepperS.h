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

#ifndef __AVR_ATmega328P__
	#error !!This library only supports the ATmega328p MCU!!
#endif



#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>
#include <uStepperEncoder.h>
#include <TMC5130.h>


/** Chip select Pin 10 on ATmega328p */
#define CS_1 PB2 

/** Pin 11 on ATmega328p (MOSI) */
#define MOSI PB3 

/** Pin 12 on ATmega328p (MISO) */
#define MISO PB4 

/** Pin 13 on ATmega328p (SCK) */
#define SCK PB5 



/**
 * @brief	Measures angle of motor.
 *
 *			This interrupt routine is in charge of sampling the encoder
 */
extern "C" void TIMER1_COMPA_vect(void) __attribute__ ((signal,used));

class uStepperS
{

public:			
	/** Instantiate object for the encoder */
	// uStepperEncoder encoder;		

	/**
	 * @brief	Constructor of uStepper class
	 */
	uStepperS( int32_t maxVelocity, int32_t maxAcceleration );

	/**
	 * @brief	Setup function
	 */
	void setup(void);

	void runContinous(bool dir);

	void moveSteps(int32_t position);

	void setSpeed(uint32_t velocity); // In usteps / t

	void setRPM(uint16_t RPM);
	

private: 

	/** This variable contains the maximum velocity, the motor is
	 * allowed to reach at any given point. The user of the library can
	 * set this by use of the setMaxVelocity() function, and get the
	 * current value with the getMaxVelocity() function. */
	float velocity;					

	/** This variable contains the maximum acceleration to be used. The
	 * can be set and read by the user of the library using the
	 * functions setMaxAcceleration() and getMaxAcceleration()
	 * respectively. Since this library uses a second order acceleration
	 * curve, the acceleration applied will always be either +/- this
	 * value (acceleration/deceleration)or zero (cruise). */
	float acceleration;		

	int32_t position;		



	uint8_t SPI( uint8_t data );

	void setSPIMode( uint8_t mode );

	int32_t setRegisterValue( uint8_t address, uint32_t datagram );

	void setDriverProfile( uint8_t mode );

};



#endif