#include <uStepperS.h>

uStepperS * pointer;

uStepperS::uStepperS()
{
	pointer = this;

	this->setMaxAcceleration(100); 	// steps / ta^2
	this->setMaxVelocity(1000);		// steps / t

	// t = 2^24 / fCLK
	// ta² = 2^41 / (fCLK)²
}

uStepperS::uStepperS(float acceleration, float velocity)
{
	pointer = this;

	this->setMaxAcceleration(acceleration);
	this->setMaxVelocity(velocity);
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

	this->angleToStep = ((float)(200 * microSteps))/360.0;
	this->rpmToVelocity = ((float)(16777216 * fullSteps * microSteps))/(60.0 * CLOCKFREQ);
	this->stepMultiplier = 0;

	driver.init( this );

	encoder.init( this );
	encoder.setHome();

}

void uStepperS::moveSteps( int32_t steps )
{
	// Make sure we use position mode
	this->driver.setRampMode(POSITIONING_MODE);

	// Get current position
	int32_t current = this->driver.getPosition();

	// Set new position
	this->driver.setPosition( current + steps );
}



void uStepperS::moveAngle( float angle )
{
	int32_t steps;

	if(angle < 0.0)
	{
		steps = -(int32_t)((angle * angleToStep) - 0.5);
		this->moveSteps( steps ); 
	}
	else
	{
		steps = (int32_t)((angle * angleToStep) + 0.5);
		this->moveSteps( steps );
	}
}


void uStepperS::moveToAngle( float angle )
{
	float diff;
	int32_t steps;

	diff = angle - this->angleMoved();

	steps = (int32_t)( (abs(diff) * angleToStep) + 0.5);

	Serial.println(steps);

	if(diff < 0.0)
	{
		this->moveSteps( steps );
	}
	else
	{
		this->moveSteps( steps );
	}

}


void uStepperS::setRPM( float rpm)
{
	int32_t velocity = (int32_t)(rpmToVelocity * rpm);

	if(velocity < 0){
		driver.setDirection(0);
	}else{
		driver.setDirection(1);
	}

	// The velocity cannot be signed
	uint32_t velocity = abs(velocity);

	// Calculated velocity should not exceed maxVelocity 
	if(velocity > this->maxVelocity ){
		velocity = this->maxVelocity;
	}

	driver.setVelocity( velocity );
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

void uStepperS::setMaxVelocity( float velocity )
{
	this->maxVelocity = abs(velocity);
	// Steps per second, has to be converted to microsteps
	this->driver.setVelocity( (uint32_t)(this->maxVelocity * this->microSteps) );
}

void uStepperS::setMaxAcceleration( float acceleration )
{
	this->maxAcceleration = abs(acceleration);
	// Steps per second, has to be converted to microsteps
	this->driver.setAcceleration( (uint32_t)(this->maxAcceleration * this->microSteps) );
}

void uStepperS::setMaxDeceleration( float deceleration )
{
	// Steps per second, has to be converted to microsteps
	this->driver.setDeceleration( (uint32_t)(abs(deceleration * this->microSteps )) );
}

void uStepperS::setCurrent( double current )
{
	if( current >= 100.0 && current <= 0.0){
		// The current needs to be in the range of 0-31
		this->driver.current = ceil(0.31 * current); 
	}else{
		// If value is out of range, set default
		this->driver.current = 16; 
	}

	driver.updateCurrent();
}

void uStepperS::setHoldCurrent( double current )
{
	// The current needs to be in the range of 0-31
	if( current >= 100.0 && current <= 0.0){
		// The current needs to be in the range of 0-31
		this->driver.holdCurrent = ceil(0.31 * current); 
	}else{
		// If value is out of range, set default
		this->driver.holdCurrent = 16; 
	}

	driver.updateCurrent();
}


void uStepperS::runContinous( bool direction )
{
	// Make sure we use velocity mode
	this->driver.setRampMode( VELOCITY_MODE_POS );

	// Set the direction
	this->driver.setDirection( direction );
}

float uStepperS::angleMoved ( void )
{
	return this->encoder.angleMoved / this->angleToStep;
}

void TIMER1_COMPA_vect(void)
{
	uint16_t curAngle;
	int32_t deltaAngle;

	float newSpeed;
	static float deltaSpeedAngle = 0.0;
	static uint8_t loops = 0;
	static uint32_t start = micros();

	curAngle = pointer->encoder.captureAngle();

	curAngle -= pointer->encoder.encoderOffset;
	pointer->encoder.angle = curAngle;

	deltaAngle = (int32_t)pointer->encoder.oldAngle - (int32_t)curAngle;

	if(deltaAngle < -32768)
	{
		pointer->encoder.revolutions--;
		deltaAngle += 65535;
	}
	else if(deltaAngle > 32768)
	{
		pointer->encoder.revolutions++;
		deltaAngle -= 65535;
	}

	/* Calculation of speed */
	if( loops < 10 )
	{
		loops++;
		deltaSpeedAngle += (float)deltaAngle;
	}
	else
	{
		pointer->encoder.curSpeed = deltaSpeedAngle * ENCODERSPEEDCONSTANT;
		pointer->encoder.deltaTime = micros() - start; 

		loops = 0;
		deltaSpeedAngle = 0.0;

		start = micros();
	}

	pointer->encoder.angleMoved = (int32_t)curAngle + (65535 * (int32_t)pointer->encoder.revolutions );
	pointer->encoder.oldAngle = curAngle;
}
