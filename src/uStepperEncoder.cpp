#include <uStepperEncoder.h>
/* At initialition setup the SPI hardware protocal to communicate with SSI interface */

uStepperEncoder::uStepperEncoder(void)
{

	/* Prepare Hardware SPI communication */

	/* Set CS (PB2), MOSI (PB3) and SCK (PB5) as Output */
	DDRB = (1<<DIN)|(1<<CLK)|(1<<CS);

	/* 
	*  SPE   = 1: SPI enabled
	*  MSTR  = 1: Master
	*  SPR0  = 1 & SPR1 = 0: fOSC/16 = 1Mhz
	*  CPOL  = 1: Idle at HIGH
	*  CPHA  = 0: Sample at leading edge
	*/
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<CPOL);
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

	uint16_t value = 0;
	uint8_t stats = 0;

	PORTB |= (1<<CS);  // Set CS HIGH
	
	/* Write dummy and read the incoming 8 bits */
	value = this->SPI(0x00);
	value <<= 8;

	/* Write dummy and read the incoming 8 bits */
	value |= this->SPI(0x00);

	/* Write dummy and read the incoming 8 bits */
	stats = this->SPI(0x00);

	PORTB &= ~(1<<CS);  // Set CS LOW
	
	this->angle = value;

}

float uStepperEncoder::getAngle(void){
	return this->angle;
}

uint8_t uStepperEncoder::SPI(uint8_t data){

  SPDR = data;

  // Wait for transmission complete
  while(!( SPSR & (1 << SPIF) ));    
  
  return SPDR;

}
