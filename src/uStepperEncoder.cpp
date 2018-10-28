#include <uStepperS.h>
/* At initialition setup the SPI hardware protocal to communicate with SSI interface */

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

void uStepperEncoder::init(uStepperS * _pointer){
	
	this->pointer = _pointer;

	angle = 0;

	/* Set the interrupt mode to 14 with a prescaler of 1 */
	TCCR1A = (1 << WGM11);
	TCCR1B = (1 << WGM12) | (1 << WGM13) | (1 << CS10);

	/* Reset Timer1 and set compare interrupt each: 62.5 ns * 32000 = 2 milliseconds */
	TCNT1 = 0;
	ICR1 = 32000; 

	// TIFR1 = 0

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

	this->angle = 0;
	this->angleMoved = 0;
	this->revolutions = 0;

	sei();
}

uint16_t uStepperEncoder::captureAngle(void){

	pointer->setSPIMode(2);

	uint16_t value = 0;
	uint8_t stats = 0;

	chipSelect(true);  // Set CS HIGH
	delayMicroseconds(1);
	
	/* Write dummy and read the incoming 8 bits */
	value = pointer->SPI(0x00);
	value <<= 8;

	/* Write dummy and read the incoming 8 bits */
	value |= pointer->SPI(0x00);

	/* Write dummy and read the incoming 8 bits */
	stats = pointer->SPI(0x00);

	chipSelect(false);  // Set CS LOW
	
	return value;
}

float uStepperEncoder::getAngle(void){
	return ((float)angle / 65536.0) * 360.0;
}

uint16_t uStepperEncoder::getRaw(void){
	return angle;
}


void uStepperEncoder::chipSelect(bool state){
	if(state)
		PORTD |= (1 << CS_ENCODER); // Set CS HIGH
	else
		PORTD &= ~(1 << CS_ENCODER);  // Set CS LOW 
}