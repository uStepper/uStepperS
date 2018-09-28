#include <uStepperS.h>
/* At initialition setup the SPI hardware protocal to communicate with SSI interface */

uStepperEncoder::uStepperEncoder(uStepperS * _pointer)
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

	this->pointer = _pointer;
}

void uStepperEncoder::setup(void){

	/* Set the interrupt mode to 14 with a prescaler of 1 */
	TCCR1A = (1 << WGM11);
	TCCR1B = (1 << WGM12) | (1 << WGM13) | (1 << CS10);

	/* Reset Timer1 and set compare interrupt each: 62.5 ns * 32000 = 2 milliseconds */
	TCNT1 = 0;
	ICR1 = 32000; 

	// TIFR1 = 0

	/* Enable Timer1 compare interrupt */
	TIMSK1 = (1 << OCIE1A);

	/* Enable global interrupts */
	sei();
}


void uStepperEncoder::captureAngle(void){

	this->pointer->setSPIMode(2);

	uint16_t value = 0;
	uint8_t stats = 0;

	this->chipSelect(true);  // Set CS HIGH
	
	/* Write dummy and read the incoming 8 bits */
	value = this->pointer->SPI(0x00);
	value <<= 8;

	/* Write dummy and read the incoming 8 bits */
	value |= this->pointer->SPI(0x00);

	/* Write dummy and read the incoming 8 bits */
	stats = this->pointer->SPI(0x00);

	this->chipSelect(false);  // Set CS LOW
	
	this->angle = value;

}

float uStepperEncoder::getAngle(void){
	return this->angle;
}


void uStepperEncoder::chipSelect(bool state){
	if(state == false)
		PORTD &= ~(1 << CS_ENCODER);  // Set CS LOW 
	else
		PORTD |= (1 << CS_ENCODER); // Set CS HIGH
}