#include <uStepperS.h>

uStepperS * pointer;

uStepperS::uStepperS( int32_t maxVelocity, int32_t maxAcceleration )
{

	// this->encoder.setup();

	acceleration = maxAcceleration;
	velocity = maxVelocity;




	pointer = this;
}

void uStepperS::setup( void )
{
	/** Prepare hardware SPI */

	/* Set CS, MOSI, SCK and DRV_ENN as Output */
	DDRB = (1<<MOSI1)|(1<<CS_DRIVER);
	DDRC = (1<<SCK1);
	DDRD = (1<<DRV_ENN);


	PORTD &= ~(1 << DRV_ENN);  // Set DRV_ENN LOW  

	/* 
	*  SPE   = 1: SPI enabled
	*  MSTR  = 1: Master
	*  SPR0  = 1 & SPR1 = 0: fOSC/16 = 1Mhz
	*/
	SPCR1 = (1<<SPE1)|(1<<MSTR1)|(1<<SPR01);
	
	setSPIMode(3);


	driver = new uStepperDriver( this) ;
	driver->setup(5,5);
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
			SPCR1 |= (1<<CPOL1);  // Set CPOL HIGH
			SPCR1 &= ~(1<<CPHA1);  // Set CPHA LOW
		break;

		case 3:
			SPCR1 |= (1<<CPOL1);  // Set CPOL HIGH
			SPCR1 |= (1<<CPHA1);  // Set CPHA HIGH
		break;
	}
}


uint8_t uStepperS::SPI(uint8_t data){

	SPDR1 = data;

	// Wait for transmission complete
	while(!( SPSR1 & (1 << SPIF1) ));    

	return SPDR1;

}


void TIMER1_COMPA_vect(void){

	// pointer->encoder.captureAngle();

}
