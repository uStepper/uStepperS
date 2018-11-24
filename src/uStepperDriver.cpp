#include <uStepperS.h>

uStepperDriver::uStepperDriver( void ){
}


void uStepperDriver::reset( void ){

	this->writeRegister(XACTUAL, 0);
	this->writeRegister(XTARGET, 0);

	this->writeRegister( IHOLD_IRUN,0 );
	this->writeRegister( CHOPCONF, 	0 );
	this->writeRegister( GCONF, 	0 );
	this->writeRegister( PWMCONF, 	0 );	
	this->writeRegister( TPWMTHRS, 	0 );

	this->writeRegister( RAMPMODE, 	0 );
	this->writeRegister( VSTART, 	0 );
	this->writeRegister( A1, 		0 ); 
	this->writeRegister( V1, 		0 ); 
	this->writeRegister( AMAX, 		0 );
	this->writeRegister( VMAX, 		0 );
	this->writeRegister( D1, 		0 );
	this->writeRegister( VSTOP, 	0 );

}

void uStepperDriver::init( uStepperS * _pointer ){

	this->pointer = _pointer;
	this->chipSelect(true); // Set CS HIGH

	// First clear previous defined registers
	this->reset();

	/* Set motor current */
	this->writeRegister( IHOLD_IRUN, IHOLD( this->holdCurrent) | IRUN( this->current) | IHOLDDELAY( this->holdDelay) );

	// this->enableStealth( 100000 );

	/* Set all-round chopper configuration */
	this->writeRegister( CHOPCONF, TOFF(4) | TBL(2) | HSTRT_TFD(4) | HEND(0) );

	/* Set startup ramp mode */
	this->setRampMode( VELOCITY_MODE_POS );

	/* Reset position */
	this->writeRegister(XACTUAL, 0);
	this->writeRegister(XTARGET, 0);

}

void uStepperDriver::setVelocity( uint32_t velocity )
{
	this->VMAX = velocity;
	this->writeRegister(VMAX_REG, this->VMAX);
}

void uStepperDriver::setAcceleration( uint16_t acceleration )
{
	this->AMAX = acceleration;
	this->writeRegister(AMAX_REG, this->AMAX);
}

void uStepperDriver::setDeceleration( uint16_t deceleration )
{
	this->DMAX = deceleration;
	this->writeRegister(DMAX_REG, this->DMAX);
}

void uStepperDriver::setCurrent( uint8_t current )
{
	this->current = current;
	this->updateCurrent();
}

void uStepperDriver::setHoldCurrent( uint8_t current )
{
	this->holdCurrent = current;
	this->updateCurrent();
}

void uStepperDriver::updateCurrent( void )
{
	this->writeRegister( IHOLD_IRUN, IHOLD( this->holdCurrent) | IRUN( this->current) | IHOLDDELAY( this->holdDelay) );
}

void uStepperDriver::setPosition( int32_t position )
{
	this->setRampMode(POSITIONING_MODE);
	this->writeRegister(XTARGET, position);
}

void uStepperDriver::setShaftDirection( bool direction )
{
	// Read the register to save the settings
	int32_t value = this->readRegister( GCONF );

	// Update the direction bit
	if(direction == 1){
		value |= DIRECTION(1);
	}else{
		value &= ~(DIRECTION(1));
	}

	this->writeRegister( GCONF, value ); 
}

void uStepperDriver::setDirection( bool direction )
{
	if(direction){
		this->writeRegister( RAMPMODE, VELOCITY_MODE_POS ); 
	}else{
		this->writeRegister( RAMPMODE, VELOCITY_MODE_NEG ); 
	}
}

void uStepperDriver::setRampProfile( uint32_t speed, uint16_t acceleration, uint16_t deceleration ){

	this->V1 = speed;
	this->A1 = acceleration;
	this->D1 = deceleration;

}

void uStepperDriver::setRampMode( uint8_t mode ){

	switch(mode){
		case POSITIONING_MODE:
			// Positioning mode
			this->writeRegister(VSTART_REG, this->VSTART);
			this->writeRegister(A1_REG, 	this->A1); 					
			this->writeRegister(V1_REG, 	this->V1); 					
			this->writeRegister(AMAX_REG, 	this->AMAX); 	
			this->writeRegister(VMAX_REG, 	this->VMAX);
			this->writeRegister(DMAX_REG, 	this->DMAX); 	
			this->writeRegister(D1_REG, 	this->D1); 					
			this->writeRegister(VSTOP_REG, 	this->VSTOP); /* Minimum 10 in POSITIONING_MODE */				
			this->writeRegister(RAMPMODE, POSITIONING_MODE); /* RAMPMODE = POSITIONING_MODE */
		break;

		case VELOCITY_MODE_POS:
			// Velocity mode (only AMAX and VMAX is used)
			this->writeRegister(VSTART_REG, this->VSTART);
			this->writeRegister(A1_REG, 	0); 						
			this->writeRegister(V1_REG, 	0); 
			this->writeRegister(AMAX_REG, 	this->AMAX); 	
			this->writeRegister(VMAX_REG, 	this->VMAX);
			this->writeRegister(DMAX_REG, 	0); 
			this->writeRegister(D1_REG, 	0); 					
			this->writeRegister(VSTOP_REG, 	0); 		
			this->writeRegister(RAMPMODE, VELOCITY_MODE_POS); /* RAMPMODE = VELOCITY_MODE_POS */
		break;
	}
}

void uStepperDriver::enableStealth( uint32_t threshold )
{
	/* Set GCONF and enable stealthChop */
	this->writeRegister( GCONF, EN_PWM_MODE(1) | I_SCALE_ANALOG(1) | DIRECTION(1) ); 

	/* Set PWMCONF for StealthChop */
	this->writeRegister( PWMCONF, PWM_GRAD(1) | PWM_AMPL(255) | PWM_FREQ(0) | FREEWHEEL(1) ); 

	/* Specifies the upper velocity for operation in stealthChop voltage PWM mode */
	this->writeRegister( TPWMTHRS, threshold ); 
}

int32_t uStepperDriver::getVelocity( void )
{
	return this->readRegister(VACTUAL);
}

int32_t uStepperDriver::getPosition( void )
{
	return this->readRegister(XACTUAL);
}

void uStepperDriver::stop( void )
{
	this->setVelocity(0);
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

	sei(); 

	return package;
}

int32_t uStepperDriver::readRegister( uint8_t address )
{
	// Disabled interrupts until write is complete
	cli();

	// Enable SPI mode 3 to use TMC5130
	this->pointer->setSPIMode(3);

	// Request a reading on address
	this->chipSelect(false);
	this->status = this->pointer->SPI(address);
	this->pointer->SPI(0x00);
	this->pointer->SPI(0x00);
	this->pointer->SPI(0x00);
	this->pointer->SPI(0x00);
	this->chipSelect(true);

	// Read the actual value on second request
	int32_t value = 0;

	this->chipSelect(false);
	this->status = this->pointer->SPI(address);
	value |= this->pointer->SPI(0x00);
	value <<= 8;
	value |= this->pointer->SPI(0x00);
	value <<= 8;
	value |= this->pointer->SPI(0x00);
	value <<= 8;
	value |= this->pointer->SPI(0x00);
	this->chipSelect(true);

	sei(); 

	return value;
}

void uStepperDriver::chipSelect(bool state)
{
	if(state == false)
		PORTE &= ~(1 << CS_DRIVER);  // Set CS LOW 
	else
		PORTE |= (1 << CS_DRIVER); // Set CS HIGH

	if( state )
		delayMicroseconds(100);   // per spec, settling time is 100us
}