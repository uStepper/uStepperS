#include <uStepperS.h>
uStepperS * pointer;
volatile float debug, debug1;
uStepperS::uStepperS()
{
	pointer = this;

	this->microSteps = 256;
	this->init();	

	this->setMaxAcceleration(2000.0);
	this->setMaxDeceleration(2000.0);
	this->setMaxVelocity(100.0);
}

uStepperS::uStepperS(float acceleration, float velocity)
{
	pointer = this;
	this->microSteps = 256;
	this->init();

	this->setMaxAcceleration(acceleration);
	this->setMaxVelocity(velocity);
}


void uStepperS::init( void ){

	
	this->pidDisabled = 1;
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
	*  SPR0  = 0 & SPR1 = 0: fOSC/4 = 4Mhz
	*/
	SPCR1 = (1<<SPE1)|(1<<MSTR1);	

	

	driver.init( this );
	encoder.init( this );
}

bool uStepperS::getMotorState(void)
{
	this->driver.readMotorStatus();
	if(this->driver.status & 0x20)
	{
		return 0;
	}
	return 1;
}

void uStepperS::setup(	uint8_t mode, 
							uint16_t stepsPerRevolution,
							float pTerm, 
							float iTerm,
							float dTerm,
							uint16_t dropinStepSize,
							bool setHome)
{
	// Should setup mode etc. later
	this->mode = mode;
	this->fullSteps = stepsPerRevolution;
	this->dropinStepSize = 256/dropinStepSize;
	this->angleToStep = (float)(this->fullSteps*this->microSteps)/360.0;
	this->rpmToVelocity = (float)(279620.267 * fullSteps * microSteps)/(CLOCKFREQ);
	this->stepsPerSecondToRPM = 60.0/(this->microSteps*this->fullSteps);
	this->RPMToStepsPerSecond = (this->microSteps*this->fullSteps)/60.0;
	this->init();

	this->setCurrent(40.0);
	this->setHoldCurrent(25.0);

	while(this->driver.readRegister(VACTUAL) != 0);

	delay(500);

	encoder.setHome();

	this->moveAngle(0.8);

	while(this->getMotorState());

	if(this->encoder.getAngleMoved() < 0.0)
	{
		this->driver.setShaftDirection(1);
	}
	else
	{
		this->driver.setShaftDirection(0);
	}
	
	encoder.setHome();

	if(this->mode)
	{
		if(this->mode == DROPIN)
		{
			//Set Enable, Step and Dir signal pins from 3dPrinter controller as inputs
			pinMode(2,INPUT);		
			pinMode(3,INPUT);
			pinMode(4,INPUT);
			//Enable internal pull-up resistors on the above pins
			digitalWrite(2,HIGH);
			digitalWrite(3,HIGH);
			digitalWrite(4,HIGH);
			attachInterrupt(0, interrupt0, FALLING);
			attachInterrupt(1, interrupt1, CHANGE);
			this->driver.writeRegister(AMAX_REG, 	65535); 
			this->driver.writeRegister(DMAX_REG, 	65535); 
		}		

		//Scale supplied controller coefficents. This is done to enable the user to use easier to manage numbers for these coefficients.
	    this->pTerm = pTerm; 
	    this->iTerm = iTerm * ENCODERINTPERIOD;    
	    this->dTerm = dTerm * ENCODERINTFREQ;    
	}
	this->pidDisabled = 0;

	DDRB |= (1 << 4);
}

void uStepperS::moveSteps( int32_t steps )
{
	this->driver.setDeceleration( (uint16_t)( this->maxDeceleration ) );
	this->driver.setAcceleration( (uint16_t)(this->maxAcceleration ) );
	this->driver.setVelocity( (uint32_t)( this->maxVelocity  ) );
	
	// Get current position
	int32_t current = this->driver.getPosition();

	// Set new position
	this->driver.setPosition( current + steps);
}



void uStepperS::moveAngle( float angle )
{
	int32_t steps;

	if(angle < 0.0)
	{
		steps = (int32_t)((angle * angleToStep) - 0.5);
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
		this->moveSteps( -steps );
	}
	else
	{
		this->moveSteps( steps );
	}
}
bool uStepperS::isStalled( void )
{
	int32_t driverSpeed;
	static uint8_t count = 0;
	float angle;
	static float oldAngle;
	float speed;
	int32_t driverPos;
	static int32_t oldDriverPos = 0;
	int32_t diffAngle;
	int32_t diffDriverPos;

	driverPos = this->driver.getPosition();
	speed = this->encoder.getSpeed();
	angle = this->encoder.getAngleMoved();

	diffAngle = (int32_t)(abs(oldAngle - angle) * this->angleToStep);
	diffDriverPos = abs(oldDriverPos - driverPos);
	oldAngle = angle;
	oldDriverPos = driverPos;

	

	if(this->driver.mode == DRIVER_POSITION)
	{
		if(abs(diffDriverPos / 2) > abs(diffAngle))
		{
			return 1;
		}
	}

	else if(!(this->driver.status & 0x08))		//Motor running
	{
		driverSpeed = (this->driver.readRegister(VACTUAL));
		
		if(driverSpeed & 0x00800000)
		{
			driverSpeed |= 0xFF000000;
		}
	    if(driverSpeed > 0)
	    {
	    	if(0.5 * (float)driverSpeed > speed)
	    	{
	    		return 1;
	    	}
	    }
	    else
	    {
	    	if(0.5 * (float)driverSpeed < speed)
	    	{
	    		return 1;
	    	}
	    }
		return 0;
	}
	
	return 0;
}

void uStepperS::brakeMotor( bool brake )
{

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

	driver.setVelocity( (uint32_t)velocity );
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
	if( abs(velocity * this->microSteps) > (float)(0xFFFFFF))
	{
		velocity = (float)0xFFFFFF;
	}
	this->maxVelocity = abs( velocity * this->microSteps );
	// Steps per second, has to be converted to microsteps
	this->driver.setVelocity( (uint32_t)( this->maxVelocity  ) );
}

void uStepperS::setMaxAcceleration( float acceleration )
{
	if( abs(acceleration) > (float)0xFFFF)
	{
		acceleration = (float)0xFFFF;
	}
	this->maxAcceleration = abs( acceleration );
	// Steps per second, has to be converted to microsteps
	this->driver.setAcceleration( (uint16_t)(this->maxAcceleration ) );
}

void uStepperS::setMaxDeceleration( float deceleration )
{
	if( abs(deceleration) > (float)0xFFFF)
	{
		deceleration = (float)0xFFFF;
	}
	this->maxDeceleration = abs( deceleration );
	// Steps per second, has to be converted to microsteps
	this->driver.setDeceleration( (uint16_t)( this->maxDeceleration ) );
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
	this->driver.setDeceleration( (uint16_t)( this->maxDeceleration ) );
	this->driver.setAcceleration( (uint16_t)(this->maxAcceleration ) );
	this->driver.setVelocity( (uint32_t)( this->maxVelocity  ) );

	// Make sure we use velocity mode
	this->driver.setRampMode( VELOCITY_MODE_POS );

	// Set the direction
	this->driver.setDirection( direction );
}

float uStepperS::angleMoved ( void )
{
	return this->encoder.getAngleMoved();
}

void uStepperS::stop( void ){

	// Check which mode is used

	// if positioning mode  
		// Update XTARGET to current postion
	// else
		// Set VMAX = 0

	// Side 76 TMC5130

	this->setRPM(0);
}

void uStepperS::filterSpeedPos(posFilter_t *filter, int32_t steps)
{
	filter->posEst += filter->velEst * ENCODERINTPERIOD;
	filter->posError = (float)steps - filter->posEst;
	filter->velIntegrator += filter->posError * PULSEFILTERKI;
	filter->velEst = (filter->posError * PULSEFILTERKP) + filter->velIntegrator;
}

void interrupt1(void)
{
	if(PIND & 0x04)
	{
		PORTD |= (1 << 4);
	}
	else
	{
		PORTD &= ~(1 << 4);
	}
}

void interrupt0(void)
{
	if(PIND & 0x04)
	{
		PORTD |= (1 << 4);
	}
	else
	{
		PORTD &= ~(1 << 4);
	}
	if((PINB & (0x08)))			//CCW
	{	
		pointer->stepCnt-=pointer->dropinStepSize;				//DIR is set to CCW, therefore we subtract 1 step from step count (negative values = number of steps in CCW direction from initial postion)
	}
	else						//CW
	{
		pointer->stepCnt+=pointer->dropinStepSize;			//DIR is set to CW, therefore we add 1 step to step count (positive values = number of steps in CW direction from initial postion)	
	}
	debug1 =  pointer->stepCnt;
}

void TIMER1_COMPA_vect(void)
{
	uint16_t curAngle;
	int32_t deltaAngle;
	int32_t stepsMoved;
	int32_t stepCntTemp;
	float error;
	float output;
	sei();
	
	curAngle = pointer->encoder.captureAngle();
	stepsMoved = pointer->driver.getPosition();
	
	curAngle -= pointer->encoder.encoderOffset;
	pointer->encoder.angle = curAngle;

	deltaAngle = (int32_t)pointer->encoder.oldAngle - (int32_t)curAngle;
	pointer->encoder.oldAngle = curAngle;

	if(deltaAngle < -32768)
	{
		deltaAngle += 65535;
	}
	else if(deltaAngle > 32768)
	{
		deltaAngle -= 65535;
	}
	pointer->encoder.angleMoved += deltaAngle;
	pointer->filterSpeedPos(&pointer->encoder.encoderFilter, pointer->encoder.angleMoved);

	if(pointer->mode == DROPIN)
	{	
		cli();
			stepCntTemp = pointer->stepCnt;
		sei();

		pointer->filterSpeedPos(&pointer->externalStepInputFilter, stepCntTemp);

		if(!pointer->pidDisabled)
		{
			error = pointer->externalStepInputFilter.posEst - (pointer->encoder.encoderFilter.posEst * ENCODERDATATOSTEP);
			pointer->currentPidSpeed = pointer->externalStepInputFilter.velIntegrator * pointer->stepsPerSecondToRPM;
			pointer->setRPM(pointer->pid(error));
		}

	}
	else if(pointer->mode == PID)
	{
		if(!pointer->pidDisabled)
		{
			error = (float)stepsMoved - (pointer->encoder.encoderFilter.posEst * ENCODERDATATOSTEP);
			
			
			if(error < -10.0 || error > 10.0)
			{
				output = pointer->pid(error);
				pointer->driver.setVelocity(abs(output) * pointer->rpmToVelocity);
				pointer->driver.writeRegister(XACTUAL,pointer->encoder.encoderFilter.posEst * ENCODERDATATOSTEP);
				pointer->driver.writeRegister(XTARGET,pointer->driver.xTarget);
			}
			else
			{
				pointer->currentPidSpeed = pointer->encoder.encoderFilter.velIntegrator * ENCODERDATATOSTEP;
				output = pointer->pid(error);
			}
		}
	}
}

void uStepperS::enablePid(void)
{
	cli();
	this->pidDisabled = 0;
	sei();
}

void uStepperS::disablePid(void)
{
	cli();
	this->pid(0.0,1);	
	this->pidDisabled = 1;
	sei();
}

float uStepperS::moveToEnd(bool dir)
{
	float length = this->encoder.getAngleMoved();

	if(dir == CW)
	{
		this->setRPM(10);
	}
	else
	{
		this->setRPM(-10);
	}
	delay(1000);
	while(!this->isStalled())
	{
	}
	this->stop();

	length -= this->encoder.getAngleMoved();
	return abs(length);
}

float uStepperS::pid(float error, bool reset)
{
	static float output;
	static float accumError = 0.0;
	static bool integralReset = 0;
	static float oldError = 0.0;
	float integral;
	static float d = 0.0;

	if(reset)
	{
		output = 0.0;
		accumError = 0.0;
		integralReset = 0;
		oldError = 0.0;
		d = 0.0;
		this->setMaxVelocity(this->maxVelocity);
		return output;
	}

	if(error > -30.0 && error < 30.0)
	{
		//accumError = 0.0;
	}
	else
	{
		if(!(error > 100.0 || error < -100.0))
		{
			//accumError *= 0.3;
		}
	}

	integral = this->iTerm * error;
	accumError += integral;

	if(accumError > 100000.0)
	{
		accumError = 100000.0;
	}
	else if(accumError < -100000.0)
	{
		accumError = -100000.0;
	}

	output = this->pTerm*error;
	if(output > 100000.0)
	{
		output = 100000.0;
	}
	else if(output < -100000.0)
	{
		output = -100000.0;
	}
	
	d *= 0.9;
	d += (this->dTerm*(error - oldError))*0.1;

	output += d;
	output += accumError;
	oldError = error;

	output *= this->stepsPerSecondToRPM;
	debug=error;
	//output += this->currentPidSpeed;
	return output;
}