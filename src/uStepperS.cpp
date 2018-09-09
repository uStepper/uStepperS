#include <uStepperS.h>

uStepperS * pointer;

uStepperS::uStepperS( int32_t maxVelocity, int32_t maxAcceleration )
{

	// this->encoder.setup();

	acceleration = maxAcceleration;
	velocity = maxVelocity;


	/** Prepare hardware SPI */

	/* Set CS (PB2), MOSI (PB3) and SCK (PB5) as Output */
	DDRB = (1<<MOSI)|(1<<CLK)|(1<<CS_DRIVER);

	/* 
	*  SPE   = 1: SPI enabled
	*  MSTR  = 1: Master
	*  SPR0  = 1 & SPR1 = 0: fOSC/16 = 1Mhz
	*/
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	
	setSPIMode(3);


	driver = new uStepperDriver( * this) ;
	driver->setup();

	pointer = this;
}


void uStepperS::moveSteps( int32_t steps ){

	position = driver->setPosition(steps + position);

}



void uStepperS::setRPM(uint16_t RPM){

	/* Constant calculated with 13,2Mhz interval oscilator, 200 steps per rev and 256 microsteps */

	uint32_t speed = 1084.587701 * (uint32_t)RPM;
	Serial.println("Speed: " + String(speed) + " RPM: " + String(RPM));
	driver->setSpeed(speed);

}


void uStepperS::setSPIMode( uint8_t mode ){
	switch(mode){
		case 2:
			SPCR |= (1<<CPOL);  // Set CPOL HIGH
			SPCR &= ~(1<<CPHA);  // Set CPHA LOW
		break;

		case 3:
			SPCR |= (1<<CPOL);  // Set CPOL HIGH
			SPCR |= (1<<CPHA);  // Set CPHA HIGH
		break;
	}
}



uint8_t uStepperS::SPI(uint8_t data){

	SPDR = data;

	// Wait for transmission complete
	while(!( SPSR & (1 << SPIF) ));    

	return SPDR;

}

void uStepperS::chipSelect(uint8_t pin, bool state){

	if(state == 0)
		PORTB &= ~(1 << pin);  // Set CS LOW 
	else
		PORTB |= (1 << pin); // Set CS HIGH

	if( state )
		delayMicroseconds(100);   // per spec, settling time is 100us
}


void TIMER1_COMPA_vect(void){

	// pointer->encoder.captureAngle();

}
