/********************************************************************************************
* 	 	File: 		uStepperEncoder.cpp														*
*		Version:    2.2.0                                           						*
*      	Date: 		September 22nd, 2020  	                                    			*
*      	Authors: 	Thomas Hørring Olsen                                   					*
*					Emil Jacobsen															*
*                                                   										*	
*********************************************************************************************
*	(C) 2020																				*
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
	if(pointer->mode == DROPIN)
	{
		ICR1 = 16000; 
	}
	else{
		ICR1 = 8000;
	}
	

	TIFR1 = 0;

	/* Enable Timer1 compare interrupt */
	TIMSK1 = (1 << OCIE1A);

	/* As long as we only use SSI, the MOSI_ENC/DIN (NSL) should be pulled LOW  */
	PORTC &= ~(1 << MOSI_ENC);  

	/* Enable global interrupts */
	sei();
}

void uStepperEncoder::setHome(float initialAngle)
{
	cli();
	TCNT1 = 0;
	this->encoderOffset = this->captureAngle();
	this->oldAngle = 0;
	this->angle = 0;
	this->angleMoved = ANGLETOENCODERDATA * initialAngle;
	this->angleMovedRaw=this->angleMoved;
	this->smoothValue = this->angleMoved;
	pointer->driver.setHome(this->angleMoved * ENCODERDATATOSTEP);
	this->encoderFilter.posError = 0.0;
	this->encoderFilter.posEst = 0.0;
	this->encoderFilter.velIntegrator = 0.0;
	this->encoderFilter.velEst = 0.0;
	this->speedSmoothValue = 0.0;
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
	int32_t deltaAngle;
	uint16_t curAngle;

	chipSelect(true);  // Set CS HIGH
	
	/* Write dummy and read the incoming 8 bits */
	value = pointer->SPI(0x00);
	value <<= 8;

	/* Write dummy and read the incoming 8 bits */
	value |= pointer->SPI(0x00);
	/* Write dummy and read the incoming 8 bits */
	this->status = pointer->SPI(0x00);

	chipSelect(false);  // Set CS LOW

	curAngle = value;
	curAngle -= this->encoderOffset;
	this->angle = curAngle;

	deltaAngle = (int32_t)this->oldAngle - (int32_t)curAngle;
	this->oldAngle = curAngle;

	if(deltaAngle < -32768)
	{
		deltaAngle += 65536;
	}
	else if(deltaAngle > 32768)
	{
		deltaAngle -= 65536;
	}

	angleMovedRaw += deltaAngle;
	pointer->driver.readRegister(VACTUAL);
	this->smoothValue = (this->smoothValue<< this->Beta)-this->smoothValue; 
   	this->smoothValue += angleMovedRaw;
   	this->smoothValue >>= this->Beta;

	if(pointer->mode != DROPIN)
	{
		this->speedSmoothValue *= 0.99;
		this->speedSmoothValue += (this->smoothValue-this->angleMoved)*0.01;
		pointer->encoder.encoderFilter.velIntegrator = this->speedSmoothValue*ENCODERINTFREQ*2.0f;
	}
   	
	this->angleMoved=this->smoothValue;

	return (uint16_t)value;
	
}

float uStepperEncoder::getAngle(void)
{
	return (float)angle * 0.005493164;	//360/65536  0.087890625
}

uint16_t uStepperEncoder::getAngleRaw(void)
{
	return angle;
}


float uStepperEncoder::getAngleMoved(bool filtered)
{
	if(filtered == true)
	{
		return this->angleMoved * 0.005493164;	//360/65536
	}
	else
	{
		return this->angleMovedRaw * 0.005493164;	//360/65536
	}
	
	
}

int32_t uStepperEncoder::getAngleMovedRaw(bool filtered)
{
	if(filtered == true)
	{
		return this->angleMoved;
	}
	else
	{
		return this->angleMovedRaw;
	}
	
	
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