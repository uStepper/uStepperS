/********************************************************************************************
* 	 	File: 		uStepperS.cpp															*
*		Version:    1.0.1                                           						*
*      	Date: 		May 14th, 2019  	                                    				*
*      	Author: 	Thomas Hørring Olsen                                   					*
*                                                   										*	
*********************************************************************************************
*	(C) 2019																				*
*																							*
*	uStepper ApS																			*
*	www.ustepper.com 																		*
*	administration@ustepper.com 															*
*																							*
*	The code contained in this file is released under the following open source license:	*
*																							*
*			Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International			*
* 																							*
* 	The code in this file is provided without warranty of any kind - use at own risk!		*
* 	neither uStepper ApS nor the author, can be held responsible for any damage				*
* 	caused by the use of the code contained in this file ! 									*
*                                                                                           *
********************************************************************************************/
/**
* @file uStepperS.cpp
*
* @brief      Function and class implementation for the uStepper S library
*
*             This file contains class and function implementations for the library.
*
* @author     Thomas Hørring Olsen (thomas@ustepper.com)
*/
#include <uStepperS.h>
uStepperS * pointer;

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

bool uStepperS::getMotorState(uint8_t statusType)
{
	this->driver.readMotorStatus();
	if(this->driver.status & statusType)
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
							bool setHome,
							uint8_t invert,
							uint8_t runCurrent,
							uint8_t holdCurrent,
							float hysteresis)
{
	dropinCliSettings_t tempSettings;
	this->pidDisabled = 1;
	// Should setup mode etc. later
	this->mode = mode;
	this->hysteresis = hysteresis;
	this->fullSteps = stepsPerRevolution;
	this->dropinStepSize = 256/dropinStepSize;
	this->angleToStep = (float)this->fullSteps * (float)this->microSteps / 360.0;
	this->rpmToVelocity = (float)(279620.267 * fullSteps * microSteps)/(CLOCKFREQ);
	this->stepsPerSecondToRPM = 60.0/(this->microSteps*this->fullSteps);
	this->RPMToStepsPerSecond = (this->microSteps*this->fullSteps)/60.0;
	this->init();

	this->setCurrent(0.0);
	this->setHoldCurrent(0.0);

	this->stop(HARD);

	while(this->driver.readRegister(VACTUAL) != 0);

	delay(500);

	this->setCurrent(40.0);
	this->setHoldCurrent(25.0);	

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
			delay(10000);
			attachInterrupt(0, interrupt0, FALLING);
			attachInterrupt(1, interrupt1, CHANGE);
			this->driver.setDeceleration( 0xFFFE);
			this->driver.setAcceleration( 0xFFFE );
			Serial.begin(9600);

			tempSettings.P.f = pTerm;
			tempSettings.I.f = iTerm;
			tempSettings.D.f = dTerm;
			tempSettings.invert = invert;
			tempSettings.runCurrent = runCurrent;
			tempSettings.holdCurrent = holdCurrent;
			tempSettings.checksum = this->dropinSettingsCalcChecksum(&tempSettings);

			if(tempSettings.checksum != EEPROM.read(sizeof(dropinCliSettings_t)))
			{
				this->dropinSettings = tempSettings;
				this->saveDropinSettings();
				EEPROM.put(sizeof(dropinCliSettings_t),this->dropinSettings.checksum);
				this->loadDropinSettings();
			}
			else
			{
				if(!this->loadDropinSettings())
				{
					this->dropinSettings = tempSettings;
					this->saveDropinSettings();
					EEPROM.put(sizeof(dropinCliSettings_t),this->dropinSettings.checksum);
					this->loadDropinSettings();
				}
			}

			
  			this->dropinPrintHelp();
		}		
		else
		{
			//Scale supplied controller coefficents. This is done to enable the user to use easier to manage numbers for these coefficients.
			this->pTerm = pTerm; 
			this->iTerm = iTerm * ENCODERINTPERIOD;    
			this->dTerm = dTerm * ENCODERINTFREQ;    
		}
	}

	this->moveAngle(10);

	while(this->getMotorState());

	if(this->encoder.getAngleMoved() < -5)
	{
		this->driver.setShaftDirection(1);
	}
	else
	{
		this->driver.setShaftDirection(0);
	}
	
	encoder.setHome();

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

bool uStepperS::detectStall(int32_t stepsMoved)
{
	static float oldTargetPosition;
	static float oldEncoderPosition;
	static float encoderPositionChange;
	static float targetPositionChange;
	float encoderPosition = ((float)this->encoder.angleMoved*ENCODERDATATOSTEP);
	static float internalStall = 0.0;

	encoderPositionChange *= stallSensitivity;
	encoderPositionChange += (1.0-stallSensitivity)*(oldEncoderPosition - encoderPosition);
	oldEncoderPosition = encoderPosition;

	targetPositionChange *= (1.0-stallSensitivity);
	targetPositionChange += stallSensitivity*(oldTargetPosition - stepsMoved);
	oldTargetPosition = stepsMoved;

	if(abs(encoderPositionChange) < abs(targetPositionChange)*0.5)
	{
		internalStall *= this->stallSensitivity;
		internalStall += 1.0-this->stallSensitivity;
	}
	else
	{
		internalStall *= this->stallSensitivity;
	}

	if(internalStall >= stallSensitivity)
	{
		this->stall = 1;
	}
	else
	{
		this->stall = 0;
	}
}

bool uStepperS::isStalled( float stallSensitivity )
{
	if(this->stallSensitivity > 1.0)
  	{
  		this->stallSensitivity = 1.0;
  	}
  	else if(this->stallSensitivity < 0.0)
  	{
  		this->stallSensitivity = 0.0;
  	}
  	else{
  		this->stallSensitivity = stallSensitivity;
  	}
  	
	return this->stall;
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
	velocity *= (float)this->microSteps;
	velocity = abs(velocity)*VELOCITYCONVERSION;

	this->maxVelocity = velocity;

	// Steps per second, has to be converted to microsteps
	this->driver.setVelocity( (uint32_t)( this->maxVelocity  ) );
}

void uStepperS::setMaxAcceleration( float acceleration )
{
	acceleration *= (float)this->microSteps;
	acceleration = abs(acceleration) * ACCELERATIONCONVERSION;

	this->maxAcceleration = acceleration;

	
	// Steps per second, has to be converted to microsteps
	this->driver.setAcceleration( (uint32_t)(this->maxAcceleration ) );
}

void uStepperS::setMaxDeceleration( float deceleration )
{
	deceleration *= (float)this->microSteps;
	deceleration = abs(deceleration) * ACCELERATIONCONVERSION;
	
	this->maxDeceleration = deceleration;
	
	// Steps per second, has to be converted to microsteps
	this->driver.setDeceleration( (uint32_t)(this->maxDeceleration ) );
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
	this->driver.setDeceleration( (uint32_t)( this->maxDeceleration ) );
	this->driver.setAcceleration( (uint32_t)(this->maxAcceleration ) );
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

void uStepperS::stop( bool mode){

	// Check which mode is used

	// if positioning mode  
		// Update XTARGET to current postion
	// else
		// Set VMAX = 0

	// Side 76 TMC5130

	if(mode == HARD)
	{
		this->driver.setDeceleration( 0xFFFE );
		this->driver.setAcceleration( 0xFFFE );
		this->setRPM(0);
		while(this->driver.readRegister(VACTUAL) != 0);
		this->driver.setDeceleration( (uint32_t)( this->maxDeceleration ) );
		this->driver.setAcceleration( (uint32_t)(this->maxAcceleration ) );
	}
	else
	{
		this->setRPM(0);
	}
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
		if(!pointer->invertPidDropinDirection)
		{
			pointer->stepCnt-=pointer->dropinStepSize;				//DIR is set to CCW, therefore we subtract 1 step from step count (negative values = number of steps in CCW direction from initial postion)
		}
		else
		{
			pointer->stepCnt+=pointer->dropinStepSize;			//DIR is set to CW, therefore we add 1 step to step count (positive values = number of steps in CW direction from initial postion)	
		}
		
	}
	else						//CW
	{
		if(!pointer->invertPidDropinDirection)
		{
			pointer->stepCnt+=pointer->dropinStepSize;			//DIR is set to CW, therefore we add 1 step to step count (positive values = number of steps in CW direction from initial postion)	
		}
		else
		{
			pointer->stepCnt-=pointer->dropinStepSize;				//DIR is set to CCW, therefore we subtract 1 step from step count (negative values = number of steps in CCW direction from initial postion)
		}
	}
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

	if(pointer->mode == DROPIN)
	{	
		cli();
			stepCntTemp = pointer->stepCnt;
		sei();

		pointer->filterSpeedPos(&pointer->externalStepInputFilter, stepCntTemp/16);

		if(!pointer->pidDisabled)
		{
			error = (stepCntTemp - (int32_t)(pointer->encoder.angleMoved * ENCODERDATATOSTEP))/16;
			pointer->currentPidSpeed = pointer->externalStepInputFilter.velIntegrator;
			pointer->pid(error);
		}
		return;
	}
	pointer->filterSpeedPos(&pointer->encoder.encoderFilter, pointer->encoder.angleMoved);
	if(pointer->mode == PID)
	{
		if(!pointer->pidDisabled)
		{
			pointer->currentPidError = stepsMoved - pointer->encoder.angleMoved * ENCODERDATATOSTEP;
			if(abs(pointer->currentPidError) >= pointer->hysteresis )
			{
				pointer->driver.writeRegister(XACTUAL,pointer->encoder.angleMoved * ENCODERDATATOSTEP);
				pointer->driver.writeRegister(XTARGET,pointer->driver.xTarget);
			}
		}
	}

	pointer->detectStall(stepsMoved);
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
	this->pidDisabled = 1;
	sei();
}

float uStepperS::moveToEnd(bool dir, float stallSensitivity, uint16_t RPM)
{
	float length = this->encoder.getAngleMoved();

	if(dir == CW)
	{
		this->setRPM(RPM);
	}
	else
	{
		this->setRPM(-RPM);
	}
	delay(500);
	while(!this->isStalled(stallSensitivity))
	{
		delay(10);
	}
	this->stop();

	length -= this->encoder.getAngleMoved();
	return abs(length);
}

float uStepperS::getPidError(void)
{
	return this->currentPidError;
}

float uStepperS::pid(float error)
{
	float u;
	float limit = abs(this->currentPidSpeed) + 10000.0;
	static float integral;
	static bool integralReset = 0;
	static float errorOld, differential = 0.0;

	this->currentPidError = error;

	u = error*this->pTerm;

	if(u > 0.0)
	{
		if(u > limit)
		{
			u = limit;
		}
	}
	else if(u < 0.0)
	{
		if(u < -limit)
		{
			u = -limit;
		}
	}

	integral += error*this->iTerm;

	if(integral > 200000.0)
	{
		integral = 200000.0;
	}
	else if(integral < -200000.0)
	{
		integral = -200000.0;
	}

	if(error > -this->hysteresis && error < this->hysteresis)
	{
		if(!integralReset)
		{
			integralReset = 1;
			integral = 0;
		}
	}
	else
	{
		integralReset = 0;
	}

	u += integral;
	
	differential *= 0.9;
	differential += 0.1*((error - errorOld)*this->dTerm);

	errorOld = error;

	u += differential;

	u *= this->stepsPerSecondToRPM * 16.0;
	this->setRPM(u);
	this->driver.setDeceleration( 0xFFFE );
	this->driver.setAcceleration( 0xFFFE );
}

void uStepperS::setProportional(float P)
{
	this->pTerm = P;
}

void uStepperS::setIntegral(float I)
{
	this->iTerm = I * ENCODERINTPERIOD; 
}

void uStepperS::setDifferential(float D)
{
	this->dTerm = D * ENCODERINTFREQ;
}

void uStepperS::invertDropinDir(bool invert)
{
	this->invertPidDropinDirection = invert;
}

void uStepperS::parseCommand(String *cmd)
{
  uint8_t i = 0;
  String value;

  if(cmd->charAt(2) == ';')
  {
    Serial.println("COMMAND NOT ACCEPTED");
    return;
  }

  value.remove(0);
  /****************** SET P Parameter ***************************
  *                                                            *
  *                                                            *
  **************************************************************/
  if(cmd->substring(0,2) == String("P="))
  {
    for(i = 2;;i++)
    {
      if(cmd->charAt(i) >= '0' && cmd->charAt(i) <= '9')
      {
        value.concat(cmd->charAt(i));
      }
      else if(cmd->charAt(i) == '.')
      {
        value.concat(cmd->charAt(i));
        i++;
        break;
      }
      else if(cmd->charAt(i) == ';')
      {
        break;
      }
      else
      {
        Serial.println("COMMAND NOT ACCEPTED");
        return;
      }
    }
    
    for(;;i++)
    {
      if(cmd->charAt(i) >= '0' && cmd->charAt(i) <= '9')
      {
        value.concat(cmd->charAt(i));
      }
      else if(cmd->charAt(i) == ';')
      {
        Serial.print("COMMAND ACCEPTED. P = ");
        Serial.println(value.toFloat(),4);
        this->dropinSettings.P.f = value.toFloat();
    	this->saveDropinSettings();
        this->setProportional(value.toFloat());
        return;
      }
      else
      {
        Serial.println("COMMAND NOT ACCEPTED");
        return;
      }
    }
  }

/****************** SET I Parameter ***************************
  *                                                            *
  *                                                            *
  **************************************************************/
  else if(cmd->substring(0,2) == String("I="))
  {
    for(i = 2;;i++)
    {
      if(cmd->charAt(i) >= '0' && cmd->charAt(i) <= '9')
      {
        value.concat(cmd->charAt(i));
      }
      else if(cmd->charAt(i) == '.')
      {
        value.concat(cmd->charAt(i));
        i++;
        break;
      }
      else if(cmd->charAt(i) == ';')
      {
        break;
      }
      else
      {
        Serial.println("COMMAND NOT ACCEPTED");
        return;
      }
    }
    
    for(;;i++)
    {
      if(cmd->charAt(i) >= '0' && cmd->charAt(i) <= '9')
      {
        value.concat(cmd->charAt(i));
      }
      else if(cmd->charAt(i) == ';')
      {
        Serial.print("COMMAND ACCEPTED. I = ");
        Serial.println(value.toFloat(),4);
        this->dropinSettings.I.f = value.toFloat();
    	this->saveDropinSettings();
        this->setIntegral(value.toFloat());
        return;
      }
      else
      {
        Serial.println("COMMAND NOT ACCEPTED");
        return;
      }
    }
  }

/****************** SET D Parameter ***************************
  *                                                            *
  *                                                            *
  **************************************************************/
  else if(cmd->substring(0,2) == String("D="))
  {
    for(i = 2;;i++)
    {
      if(cmd->charAt(i) >= '0' && cmd->charAt(i) <= '9')
      {
        value.concat(cmd->charAt(i));
      }
      else if(cmd->charAt(i) == '.')
      {
        value.concat(cmd->charAt(i));
        i++;
        break;
      }
      else if(cmd->charAt(i) == ';')
      {
        break;
      }
      else
      {
        Serial.println("COMMAND NOT ACCEPTED");
        return;
      }
    }
    
    for(;;i++)
    {
      if(cmd->charAt(i) >= '0' && cmd->charAt(i) <= '9')
      {
        value.concat(cmd->charAt(i));
      }
      else if(cmd->charAt(i) == ';')
      {
        Serial.print("COMMAND ACCEPTED. D = ");
        Serial.println(value.toFloat(),4);
        this->dropinSettings.D.f = value.toFloat();
    	this->saveDropinSettings();
        this->setDifferential(value.toFloat());
        return;
      }
      else
      {
        Serial.println("COMMAND NOT ACCEPTED");
        return;
      }
    }
  }

/****************** invert Direction ***************************
  *                                                            *
  *                                                            *
  **************************************************************/
  else if(cmd->substring(0,6) == String("invert"))
  {
      if(cmd->charAt(6) != ';')
      {
        Serial.println("COMMAND NOT ACCEPTED");
        return;
      }
      if(this->invertPidDropinDirection)
      {
      	Serial.println(F("Direction normal!"));
      	this->dropinSettings.invert = 0;
    	this->saveDropinSettings();
        this->invertDropinDir(0);
        return;
      }
      else
      {
      	Serial.println(F("Direction inverted!"));
      	this->dropinSettings.invert = 1;
    	this->saveDropinSettings();
        this->invertDropinDir(1);
        return;
      }
  }

  /****************** get Current Pid Error ********************
  *                                                            *
  *                                                            *
  **************************************************************/
  else if(cmd->substring(0,5) == String("error"))
  {
      if(cmd->charAt(5) != ';')
      {
        Serial.println("COMMAND NOT ACCEPTED");
        return;
      }
      Serial.print(F("Current Error: "));
      Serial.print(this->getPidError());
      Serial.println(F(" Steps"));
  }

  /****************** Get run/hold current settings ************
  *                                                            *
  *                                                            *
  **************************************************************/
  else if(cmd->substring(0,7) == String("current"))
  {
      if(cmd->charAt(7) != ';')
      {
        Serial.println("COMMAND NOT ACCEPTED");
        return;
      }
      Serial.print(F("Run Current: "));
      Serial.print(ceil(((float)this->driver.current)/0.31));
      Serial.println(F(" %"));
      Serial.print(F("Hold Current: "));
      Serial.print(ceil(((float)this->driver.holdCurrent)/0.31));
      Serial.println(F(" %"));
  }
  
  /****************** Get PID Parameters ***********************
  *                                                            *
  *                                                            *
  **************************************************************/
  else if(cmd->substring(0,10) == String("parameters"))
  {
      if(cmd->charAt(10) != ';')
      {
        Serial.println("COMMAND NOT ACCEPTED");
        return;
      }
      Serial.print(F("P: "));
      Serial.print(this->dropinSettings.P.f,4);
      Serial.print(F(", "));
      Serial.print(F("I: "));
      Serial.print(this->dropinSettings.I.f,4);
      Serial.print(F(", "));
      Serial.print(F("D: "));
      Serial.println(this->dropinSettings.D.f,4);
  }

  /****************** Help menu ********************************
  *                                                            *
  *                                                            *
  **************************************************************/
  else if(cmd->substring(0,4) == String("help"))
  {
      if(cmd->charAt(4) != ';')
      {
        Serial.println("COMMAND NOT ACCEPTED");
        return;
      }
      this->dropinPrintHelp();
  }

/****************** SET run current ***************************
  *                                                            *
  *                                                            *
  **************************************************************/
  else if(cmd->substring(0,11) == String("runCurrent="))
  {
    for(i = 11;;i++)
    {
      if(cmd->charAt(i) >= '0' && cmd->charAt(i) <= '9')
      {
        value.concat(cmd->charAt(i));
      }
      else if(cmd->charAt(i) == '.')
      {
        value.concat(cmd->charAt(i));
        i++;
        break;
      }
      else if(cmd->charAt(i) == ';')
      {
        break;
      }
      else
      {
        Serial.println("COMMAND NOT ACCEPTED");
        return;
      }
    }
    
    for(;;i++)
    {
      if(cmd->charAt(i) >= '0' && cmd->charAt(i) <= '9')
      {
        value.concat(cmd->charAt(i));
      }
      else if(cmd->charAt(i) == ';')
      {
      	if(value.toFloat() >= 0.0 && value.toFloat() <= 100.0)
      	{
      		i = (uint8_t)value.toFloat();
    		break;	
      	}
      	else
      	{
      		Serial.println("COMMAND NOT ACCEPTED");
        	return;
      	}
      }
      else
      {
        Serial.println("COMMAND NOT ACCEPTED");
        return;
      }
    }

    Serial.print("COMMAND ACCEPTED. runCurrent = ");
    Serial.print(i);
    Serial.println(F(" %"));
    this->dropinSettings.runCurrent = i;
    this->saveDropinSettings();
    this->setCurrent(i);
  }

  /****************** SET run current ***************************
  *                                                            *
  *                                                            *
  **************************************************************/
  else if(cmd->substring(0,12) == String("holdCurrent="))
  {
    for(i = 12;;i++)
    {
      if(cmd->charAt(i) >= '0' && cmd->charAt(i) <= '9')
      {
        value.concat(cmd->charAt(i));
      }
      else if(cmd->charAt(i) == '.')
      {
        value.concat(cmd->charAt(i));
        i++;
        break;
      }
      else if(cmd->charAt(i) == ';')
      {
        break;
      }
      else
      {
        Serial.println("COMMAND NOT ACCEPTED");
        return;
      }
    }
    
    for(;;i++)
    {
      if(cmd->charAt(i) >= '0' && cmd->charAt(i) <= '9')
      {
        value.concat(cmd->charAt(i));
      }
      else if(cmd->charAt(i) == ';')
      {
      	if(value.toFloat() >= 0.0 && value.toFloat() <= 100.0)
      	{
      		i = (uint8_t)value.toFloat();
    		break;	
      	}
      	else
      	{
      		Serial.println("COMMAND NOT ACCEPTED");
        	return;
      	}
      }
      else
      {
        Serial.println("COMMAND NOT ACCEPTED");
        return;
      }
    }

    Serial.print("COMMAND ACCEPTED. holdCurrent = ");
    Serial.print(i);
    Serial.println(F(" %"));
    this->dropinSettings.holdCurrent = i;
    this->saveDropinSettings();
    this->setHoldCurrent(i);
  }

  /****************** DEFAULT (Reject!) ************************
  *                                                            *
  *                                                            *
  **************************************************************/
  else
  {
    Serial.println("COMMAND NOT ACCEPTED");
    return;
  }
  
}

void uStepperS::dropinCli()
{
	static String stringInput;
	static uint32_t t = millis();

	while(1)
	{
		while(!Serial.available())
		{
			delay(1);
			if((millis() - t) >= 500)
			{
				stringInput.remove(0);
				t = millis();
			}
		}
		t = millis();
		stringInput += (char)Serial.read();
		if(stringInput.lastIndexOf(';') > -1)
		{
		  this->parseCommand(&stringInput);
		  stringInput.remove(0);
		}
	}
}

void uStepperS::dropinPrintHelp()
{
	Serial.println(F("uStepper S Dropin !"));
	Serial.println(F(""));
	Serial.println(F("Usage:"));
	Serial.println(F("Show this command list: 'help;'"));
	Serial.println(F("Get PID Parameters: 'parameters;'"));
	Serial.println(F("Set Proportional constant: 'P=10.002;'"));
	Serial.println(F("Set Integral constant: 'I=10.002;'"));
	Serial.println(F("Set Differential constant: 'D=10.002;'"));
	Serial.println(F("Invert Direction: 'invert;'"));
	Serial.println(F("Get Current PID Error: 'error;'"));
	Serial.println(F("Get Run/Hold Current Settings: 'current;'"));
	Serial.println(F("Set Run Current (percent): 'runCurrent=50.0;'"));
	Serial.println(F("Set Hold Current (percent): 'holdCurrent=50.0;'"));
	Serial.println(F(""));
	Serial.println(F(""));
}

bool uStepperS::loadDropinSettings(void)
{
	dropinCliSettings_t tempSettings;

	EEPROM.get(0,tempSettings);

	if(this->dropinSettingsCalcChecksum(&tempSettings) != tempSettings.checksum)
	{
		return 0;
	}

	this->dropinSettings = tempSettings;
	this->setProportional(this->dropinSettings.P.f);
	this->setIntegral(this->dropinSettings.I.f);
	this->setDifferential(this->dropinSettings.D.f);
	this->invertDropinDir((bool)this->dropinSettings.invert);
	this->setCurrent(this->dropinSettings.runCurrent);	
	this->setHoldCurrent(this->dropinSettings.holdCurrent);	
	return 1;
}

void uStepperS::saveDropinSettings(void)
{
	this->dropinSettings.checksum = this->dropinSettingsCalcChecksum(&this->dropinSettings);

	EEPROM.put(0,this->dropinSettings);
}

uint8_t uStepperS::dropinSettingsCalcChecksum(dropinCliSettings_t *settings)
{
	uint8_t i;
	uint8_t checksum = 0xAA;
	uint8_t *p = (uint8_t*)settings;

	for(i=0; i < 15; i++)
	{		
		checksum ^= *p++;
	}

	return checksum;
}