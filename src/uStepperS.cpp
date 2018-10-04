#include <uStepperS.h>

uStepperS * pointer;

uStepperS::uStepperS()
{
	pointer = this;
}

void uStepperS::setup( void )
{
	/** Prepare hardware SPI */

	/* Set CS, MOSI, SCK and DRV_ENN as Output */
	DDRC = (1<<SCK1)|(1<<MOSI_ENC);
	DDRD = (1<<DRV_ENN)|(1<<SD_MODE)|(1<<CS_ENCODER);
	DDRE = (1<<MOSI1)|(1<<CS_DRIVER);

	PORTD &= ~(1 << DRV_ENN);  // Set DRV_ENN LOW  
	PORTD &= ~(1 << SD_MODE);  // Set SD_MODE LOW  
	/* 
	*  ---- Global SPI1 configuration ----
	*  SPE   = 1: SPI enabled
	*  MSTR  = 1: Master
	*  SPR0  = 1 & SPR1 = 0: fOSC/16 = 1Mhz
	*/
	SPCR1 = (1<<SPE1)|(1<<MSTR1)|(1<<SPR10);

	Serial.println(SPCR1, BIN);


	this->driver.initiate( this );
	// driver->setup(16,16);

	encoder.initiate( this );
	encoder.begin();
}

void uStepperS::moveSteps( int32_t steps ){

	position = driver.setPosition(steps + position);

}

void uStepperS::setRPM(uint16_t RPM){

	/* Constant calculated with 16Mhz external oscilator, 200 steps per rev and 256 microsteps */

	uint32_t speed = 894.785 * (uint32_t)RPM;
	// Serial.println("Speed: " + String(speed) + " RPM: " + String(RPM));
	driver.setSpeed(speed);

}


void uStepperS::setSPIMode( uint8_t mode ){

	switch(mode){
		case 2:
			SPCR1 |= (1<<CPOL1);  // Set CPOL HIGH = 1
			SPCR1 &= ~(1<<CPHA1);  // Set CPHA LOW = 0
		break;

		case 3:
			SPCR1 |= (1<<CPOL1);  // Set CPOL HIGH = 1
			SPCR1 |= (1<<CPHA1);  // Set CPHA HIGH = 1
		break;
	}
}

uint8_t uStepperS::SPI(uint8_t data){

	SPDR1 = data;

	// Wait for transmission complete
	while(!( SPSR1 & (1 << SPIF1) ));    

	return SPDR1;

}


void uStepperS::setMaxAcceleration( int32_t _acceleration ){

	this->acceleration = _acceleration;

}

void uStepperS::setMaxVelocity( int32_t _velocity ){

	this->velocity = _velocity;

}


void TIMER1_COMPA_vect(void){

	pointer->encoder.captureAngle();

}
