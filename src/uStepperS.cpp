#include <uStepperS.h>

uStepperS * pointer;

uStepperS::uStepperS()
{
	pointer = this;

	this->init();

	/*this->setMaxAcceleration(100.0);
	this->setMaxVelocity(1000.0);*/
}

uStepperS::uStepperS(float acceleration, float velocity)
{
	pointer = this;

	this->init();

	this->setMaxAcceleration(acceleration);
	this->setMaxVelocity(velocity);
}


void uStepperS::init( void ){

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

	this->angleToStep = (float)(200)/360.0;
	this->rpmToVelocity = (float)(279620.267 * fullSteps * microSteps)/(CLOCKFREQ);

	driver.init( this );
	encoder.init( this );
}

void uStepperS::setup( void )
{
	// Should setup mode etc. later
	encoder.setHome();
}

void uStepperS::moveSteps( int32_t steps )
{
	// Get current position
	int32_t current = this->driver.getPosition();

	// Set new position
	this->driver.setPosition( current + (steps * microSteps) );
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
	int32_t velocityDir = rpmToVelocity * rpm;

	if(velocityDir > 0){
		driver.setDirection(1);
	}else{
		driver.setDirection(0);
	}

	// The velocity cannot be signed
	uint32_t velocity = abs(velocityDir);

	// Calculated velocity should not exceed maxVelocity 
	if(velocity > this->maxVelocity ){
		velocity = this->maxVelocity;
		Serial.println("maxVelocity reached");
	}

	driver.setVelocity( (uint32_t)(velocity * FREQSCALE) );
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
	this->maxVelocity = abs( velocity * this->microSteps );
	// Steps per second, has to be converted to microsteps
	this->driver.setVelocity( (uint32_t)( this->maxVelocity * FREQSCALE ) );
}

void uStepperS::setMaxAcceleration( float acceleration )
{
	this->maxAcceleration = abs( acceleration * this->microSteps );
	// Steps per second, has to be converted to microsteps
	this->driver.setAcceleration( (uint16_t)(this->maxAcceleration * FREQSCALE) );
}

void uStepperS::setMaxDeceleration( float deceleration )
{
	this->maxDeceleration = abs( deceleration * this->microSteps);
	// Steps per second, has to be converted to microsteps
	this->driver.setDeceleration( (uint16_t)( this->maxDeceleration * FREQSCALE) );
}

void uStepperS::setCurrent( double current )
{
	if( current <= 100.0 && current >= 0.0){
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
	if( current <= 100.0 && current >= 0.0){
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
	return (this->encoder.angleMoved / 65536.0) * 360;
}

void uStepperS::stop( void ){

	// Check which mode is used

	// if positioning mode  
		// Update XTARGET to current postion
	// else
		// Set VMAX = 0

	// Side 76 TMC5130
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
