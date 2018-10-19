#include <uStepperS.h>

uStepperDriver::uStepperDriver( void ){
}

void uStepperDriver::initiate( uStepperS * _pointer ){

	this->pointer = _pointer;
	this->chipSelect(true); // Set CS HIGH
}

void uStepperDriver::begin(uint8_t ihold, uint8_t irun ){

	/* Prepare general driver settings */

	/* Set motor current */
	this->writeRegister( IHOLD_IRUN, IHOLD(ihold) | IRUN(irun) | IHOLDDELAY(5));

	
	this->setChopMode( 0 );
	


	/* Enable dcStep at above VDCMIN velocity */
	// writeRegister( VDCMIN, 4500 );

	/* Set DCCTRL */
	// writeRegister( DCCTRL, (DC_TIME(25) | DC_SG(4) ) );

	/* Set PWMCONF  */
	
	/* Enable position mode */
	this->setMode( 0 );

}

void uStepperDriver::setChopMode( uint8_t mode ){

	switch(mode){
		case 0: // stealthChop

			/* Set GCONF and enable stealthChop */
			this->writeRegister( GCONF, EN_PWM_MODE(1) | I_SCALE_ANALOG(1) | DIRECTION(0) ); 

			/* Set PWMCONF for StealthChop */
			this->writeRegister( PWMCONF, PWM_GRAD(1) | PWM_AMPL(255) | PWM_FREQ(0) ); 

			/* Set standard CHOPCONF ( set VHIGHCHM(1) | VHIGHFS(1) for dcStep ) */
			this->writeRegister( CHOPCONF, TOFF(4) | TBL(2) | HSTRT_TFD(4) | HEND(0) );

			// Set max velocity for stealthChop (DOESNT WORK??)
			this->writeRegister( TPWMTHRS, 100000 ); 

		break;

	}

}

void uStepperDriver::setRampMode( uint8_t mode ){

	switch(mode){
		case POSITIONING_MODE:
			// Positioning mode
			this->writeRegister(A1, 		1000); 			/* A1 = 1000 */
			this->writeRegister(V1, 		100000); 		/* V1 = 100000 usteps / t  */
			this->writeRegister(AMAX, 	pointer->acceleration); 	/* AMAX */
			this->writeRegister(VMAX, 	pointer->velocity); 		/* VMAX */
			this->writeRegister(D1, 		1400); 			/* D1 = 1400 */
			this->writeRegister(VSTOP, 	10); 			/* VSTOP = 10 */

			this->writeRegister(RAMPMODE, POSITIONING_MODE); 			/* RAMPMODE = 0 = Positioning mode */
		
		break;

		case VELOCITY_MODE_POS:
			// Velocity mode (only AMAX and VMAX is used)

			// writeRegister(THIGH, 	120000);

			this->writeRegister(AMAX, 	pointer->acceleration); 	/* AMAX */
			this->writeRegister(VMAX, 	pointer->velocity); 		/* VMAX */

			this->writeRegister(RAMPMODE, VELOCITY_MODE_POS); 			/* RAMPMODE = 1 = Velocity mode */

		break;
	}

	/* Reset position */
	this->writeRegister(XACTUAL, 0);
	this->writeRegister(XTARGET, 0);

}


int32_t uStepperDriver::writeRegister( uint8_t address, uint32_t datagram ){

	// Disabled interrupts until write is complete
	cli();

	// Enable SPI mode 3 to use TMC5130
	this->pointer->setSPIMode(3);

	uint32_t package;

	// Add the value of WRITE_ACCESS to enable register write
	address += WRITE_ACCESS;

	this->chipSelect(false);

	this->status = this->pointer->SPI(address);

	package |= this->pointer->SPI((datagram >> 24) & 0xff);
	package <<= 8;
	package |= this->pointer->SPI((datagram >> 16) & 0xff);
	package <<= 8;
	package |= this->pointer->SPI((datagram >> 8) & 0xff);
	package <<= 8;
	package |= this->pointer->SPI((datagram) & 0xff);

	this->chipSelect(true); // Set CS HIGH

	/* 
	Serial.println(package, HEX);
	Serial.println(stats, BIN);
	*/
	sei(); 

	return package;
}


int32_t uStepperDriver::readRegister( uint8_t address ){

	// Disabled interrupts until write is complete
	cli();

	// Enable SPI mode 3 to use TMC5130
	this->pointer->setSPIMode(3);

	// Request the read for the address
	this->chipSelect(false);
	this->pointer->SPI(address);
	this->pointer->SPI(0x00);
	this->pointer->SPI(0x00);
	this->pointer->SPI(0x00);
	this->pointer->SPI(0x00);
	this->chipSelect(true);

	// Read the actual value on second request
	uint32_t value = 0;

	this->chipSelect(false);
	value |= this->pointer->SPI(0x00) << 24;
	value |= this->pointer->SPI(0x00) << 16;
	value |= this->pointer->SPI(0x00) << 8;
	value |= this->pointer->SPI(0x00);
	this->chipSelect(true);

	sei(); 

	return value;

}


void uStepperDriver::setSpeed(uint32_t velocity){

	this->writeRegister(VMAX, velocity);

}

void uStepperDriver::setPosition( int32_t position ){

	this->writeRegister(XTARGET, position);

}

int32_t uStepperDriver::getPosition( void ){

	return this->readRegister(XACTUAL);

}


void uStepperDriver::moveSteps( int32_t steps ){

	// Make sure we use position mode
	this->setMode(POSITIONING_MODE);

	// Get current position
	int32_t current = this->getPosition();

	// Set new position
	this->setPosition( current + steps );

}




void uStepperDriver::chipSelect(bool state){

	if(state == false)
		PORTE &= ~(1 << CS_DRIVER);  // Set CS LOW 
	else
		PORTE |= (1 << CS_DRIVER); // Set CS HIGH

	if( state )
		delayMicroseconds(100);   // per spec, settling time is 100us
}