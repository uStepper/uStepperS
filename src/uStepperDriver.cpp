#include <uStepperS.h>

uStepperDriver::uStepperDriver( void ){
}

void uStepperDriver::initiate( uStepperS * _pointer ){

	this->pointer = _pointer;
	chipSelect(true); // Set CS HIGH
}

void uStepperDriver::begin(uint8_t ihold, uint8_t irun ){

	/* Prepare general driver settings */

	/* Set motor current */
	writeRegister( IHOLD_IRUN, IHOLD(ihold) | IRUN(irun) | IHOLDDELAY(5));

	/* Set GCONF and enable stealthChop */
	writeRegister( GCONF, EN_PWM_MODE(1) | I_SCALE_ANALOG(1) | DIRECTION(0) ); 

	/* Set PWMCONF for StealthChop */
	writeRegister( PWMCONF, PWM_GRAD(1) | PWM_AMPL(255) | PWM_FREQ(0) ); 

	/* Set standard CHOPCONF ( set VHIGHCHM(1) | VHIGHFS(1) for dcStep ) */
	writeRegister( CHOPCONF, TOFF(4) | TBL(2) | HSTRT_TFD(4) | HEND(0) );

	// Set max velocity for stealthChop (DOESNT WORK??)
	writeRegister( TPWMTHRS, 100000 ); 


	/* Enable dcStep at above VDCMIN velocity */
	// writeRegister( VDCMIN, 4500 );

	/* Set DCCTRL */
	// writeRegister( DCCTRL, (DC_TIME(25) | DC_SG(4) ) );

	/* Set PWMCONF  */
	
	/* Enable velocity mode */
	setDriverProfile(1);

}


void uStepperDriver::setDriverProfile( uint8_t mode ){

	switch(mode){
		case 0:
			// Positioning mode
			writeRegister(A1, 		1000); 			/* A1 = 1000 */
			writeRegister(V1, 		100000); 		/* V1 = 100000 usteps / t  */
			writeRegister(AMAX, 	pointer->acceleration); 	/* AMAX */
			writeRegister(VMAX, 	pointer->velocity); 		/* VMAX */
			writeRegister(D1, 		1400); 			/* D1 = 1400 */
			writeRegister(VSTOP, 	10); 			/* VSTOP = 10 */
			writeRegister(RAMPMODE, 0); 			/* RAMPMODE = 0 = Positioning mode */
		
		break;

		case 1:
			// Velocity mode (only AMAX and VMAX is used)

			// writeRegister(THIGH, 	120000);

			writeRegister(AMAX, 	pointer->acceleration); 	/* AMAX */
			writeRegister(VMAX, 	pointer->velocity); 		/* VMAX */

			writeRegister(RAMPMODE, 1); 			/* RAMPMODE = 1 = Velocity mode */
		break;
	}

	/* Reset position */
	writeRegister(XACTUAL, 0);
	writeRegister(XTARGET, 0);

}


int32_t uStepperDriver::writeRegister( uint8_t address, uint32_t datagram ){

	cli();


	pointer->setSPIMode(3);

	uint8_t stats;
	uint32_t package;

	/* 
	Serial.print(address, HEX); Serial.print(":");
	Serial.print(datagram, HEX);Serial.print("->");
	*/

	address += WRITE_ACCESS;

	chipSelect(false);

	stats = pointer->SPI(address);

	package |= pointer->SPI((datagram >> 24) & 0xff);
	package <<= 8;

	package |= pointer->SPI((datagram >> 16) & 0xff);
	package <<= 8;

	package |= pointer->SPI((datagram >> 8) & 0xff);
	package <<= 8;

	package |= pointer->SPI((datagram) & 0xff);

	chipSelect(true); // Set CS HIGH

	/* 
	Serial.println(package, HEX);
	Serial.println(stats, BIN);
	*/
	sei(); 

	return package;



}


void uStepperDriver::setSpeed(uint32_t velocity){

	writeRegister(VMAX, velocity);

}

int32_t uStepperDriver::setPosition( int32_t position ){

	return writeRegister(XTARGET, position);

}


void uStepperDriver::chipSelect(bool state){

	if(state == false)
		PORTE &= ~(1 << CS_DRIVER);  // Set CS LOW 
	else
		PORTE |= (1 << CS_DRIVER); // Set CS HIGH

	if( state )
		delayMicroseconds(100);   // per spec, settling time is 100us
}