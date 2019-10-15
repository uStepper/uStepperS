/********************************************************************************************
* 	 	File: 		uStepperEncoder.cpp														*
*		Version:    1.0.1                                           						*
*      	Date: 		May 14th, 2019  	                                    				*
*      	Author: 	Thomas Hørring Olsen                                   					*
*                                                   										*	
*********************************************************************************************
*	(C) 2019																				*
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
* @file uStepperEncoder.cpp
*
* @brief      Function implementations for the AEAT8800-Q24 Encoder
*
*             This file contains class and function implementations for the AEAT8800-Q24 Encoder.
*
* @author     Thomas Hørring Olsen (thomas@ustepper.com)
*/
#include <uStepperS.h>
/* At initialition setup the SPI hardware protocal to communicate with SSI interface */
extern uStepperS * pointer;
uStepperEncoder::uStepperEncoder(void)
{
	/* Prepare Hardware SPI communication */

	/* 
	*  SPE   = 1: SPI enabled
	*  MSTR  = 1: Master
	*  SPR0  = 1 & SPR1 = 0: fOSC/16 = 1Mhz
	*  CPOL  = 1: Idle at HIGH
	*  CPHA  = 0: Sample at leading edge
	*/
	// SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<CPOL);
}

void uStepperEncoder::init(uStepperS * _pointer)
{
	this->pointer = _pointer;

	angle = 0;

	/* Set the interrupt mode to 14 with a prescaler of 1 */
	TCCR1A = (1 << WGM11);
	TCCR1B = (1 << WGM12) | (1 << WGM13) | (1 << CS10);

	/* Reset Timer1 and set compare interrupt each: 62.5 ns * 16000 = 1 milliseconds */
	TCNT1 = 0;
	ICR1 = 16000; 

	TIFR1 = 0;

	/* Enable Timer1 compare interrupt */
	TIMSK1 = (1 << OCIE1A);

	/* As long as we only use SSI, the MOSI_ENC/DIN (NSL) should be pulled LOW  */
	PORTC &= ~(1 << MOSI_ENC);  

	/* Enable global interrupts */
	sei();
}

void uStepperEncoder::setHome(void)
{
	cli();

	this->encoderOffset = this->captureAngle();
	this->oldAngle = 0;
	this->angle = 0;
	this->angleMoved = 0;
	this->revolutions = 0;
	pointer->driver.setHome();
	this->encoderFilter.posError = 0.0;
	this->encoderFilter.posEst = 0.0;
	this->encoderFilter.velIntegrator = 0.0;
	this->encoderFilter.velEst = 0.0;
	sei();
}

bool uStepperEncoder::detectMagnet(void)
{
	uint8_t status;

	//this->captureAngle();

	status = this->getStatus();

	if((status & 0xE0) != 0x80)
	{
		return 0;
	}

	return 1;
}

uint16_t uStepperEncoder::captureAngle(void)
{
	pointer->setSPIMode(2);

	uint16_t value = 0;

	chipSelect(true);  // Set CS HIGH
	
	/* Write dummy and read the incoming 8 bits */
	value = pointer->SPI(0x00);
	value <<= 8;

	/* Write dummy and read the incoming 8 bits */
	value |= pointer->SPI(0x00);

	/* Write dummy and read the incoming 8 bits */
	this->status = pointer->SPI(0x00);

	chipSelect(false);  // Set CS LOW


	return value;
}

float uStepperEncoder::getAngle(void)
{
	return (float)angle * 0.005493164;	//360/65536
}

uint16_t uStepperEncoder::getAngleRaw(void)
{
	return angle;
}


float uStepperEncoder::getAngleMoved(void)
{
	return this->angleMoved * 0.005493164;	//360/65536
}

int32_t uStepperEncoder::getAngleMovedRaw(void)
{
	return angleMoved;
}


uint8_t uStepperEncoder::getStatus( void )
{
	return this->status;
}

float uStepperEncoder::getSpeed( void )
{
	return pointer->encoder.encoderFilter.velIntegrator * ENCODERDATATOSTEP;
}

float uStepperEncoder::getRPM( void )
{
	return pointer->encoder.encoderFilter.velIntegrator * ENCODERDATATOREVOLUTIONS;
}

void uStepperEncoder::chipSelect(bool state)
{
	if(state)
		PORTD |= (1 << CS_ENCODER); // Set CS HIGH
	else
		PORTD &= ~(1 << CS_ENCODER);  // Set CS LOW 
}