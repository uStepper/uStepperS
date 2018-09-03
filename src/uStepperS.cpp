#include <uStepperS.h>

uStepperS * pointer;

uStepperS::uStepperS( int32_t maxVelocity, int32_t maxAcceleration )
{

	// this->encoder.setup();

	acceleration = maxAcceleration;
	velocity = maxVelocity;


	/** Prepare hardware SPI */

	/* Set CS (PB2), MOSI (PB3) and SCK (PB5) as Output */
	DDRB = (1<<MOSI)|(1<<CLK)|(1<<CS);

	/* 
	*  SPE   = 1: SPI enabled
	*  MSTR  = 1: Master
	*  SPR0  = 1 & SPR1 = 0: fOSC/16 = 1Mhz
	*/
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	setSPIMode(3);


	/* Resets GCONF for TMC5130 and enable stealth */
	setRegisterValue(0x00 + WRITE_ACCESS, 0x04); 

	setRegisterValue(0x70 + WRITE_ACCESS, 1<<18); 


	/* Set CHOPCONF for TMC5130 */
	setRegisterValue(0x6C + WRITE_ACCESS, TOFF * 5 + HSTRT * (5-1) + HEND * (3-1) );


	/* IHOLD_IRUN: IHOLD=2, IRUN=10 (max.current), IHOLDDELAY=6 */
	setRegisterValue(IHOLD_IRUN  + WRITE_ACCESS, IHOLD * 2 + IRUN * 16 + IHOLDDELAY * 10);


	/* Resets PWMCONF for TMC5130 */
	setRegisterValue(0X70 + WRITE_ACCESS,0x00);

	setDriverProfile(1);

	pointer = this;
}


void uStepperS::setDriverProfile( uint8_t mode ){

	switch(mode){
		case 0:
			// Positioning mode
			setRegisterValue(A1 + WRITE_ACCESS, 	1000); 			/* A1 = 1000 */
			setRegisterValue(V1 + WRITE_ACCESS, 	100000); 		/* V1 = 100000 usteps / t  */
			setRegisterValue(AMAX + WRITE_ACCESS, 	acceleration); 	/* AMAX */
			setRegisterValue(VMAX + WRITE_ACCESS, 	velocity); 		/* VMAX */
			setRegisterValue(D1 + WRITE_ACCESS, 	1400); 			/* D1 = 1400 */
			setRegisterValue(VSTOP + WRITE_ACCESS, 	10); 			/* VSTOP = 10 */

			setRegisterValue(RAMPMODE + WRITE_ACCESS, 0); 			/* RAMPMODE = 0 = Positioning mode */
		break;

		case 1:
			// Velocity mode (only AMAX and VMAX is used)

			setRegisterValue(THIGH + WRITE_ACCESS, 200000);

			setRegisterValue(AMAX + WRITE_ACCESS, 	acceleration); 	/* AMAX */
			setRegisterValue(VMAX + WRITE_ACCESS, 	velocity); 		/* VMAX */

			setRegisterValue(RAMPMODE + WRITE_ACCESS, 1); 			/* RAMPMODE = 1 = Velocity mode */
		break;
	}

	/* Reset position */
	setRegisterValue(XACTUAL + WRITE_ACCESS, 0);
	setRegisterValue(XTARGET + WRITE_ACCESS, 0);

}

void uStepperS::setRPM(uint16_t RPM){

	/* Constant calculated with 13,2Mhz interval oscilator, 200 steps per rev and 256 microsteps */

	uint32_t speed = 1084.587701 * (uint32_t)RPM;
	Serial.println("Speed: " + String(speed) + " RPM: " + String(RPM));
	setSpeed(speed);

}

void uStepperS::setSpeed(uint32_t velocity){

	setRegisterValue(VMAX + WRITE_ACCESS, velocity);

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


int32_t uStepperS::setRegisterValue( uint8_t address, uint32_t datagram ){

	uint8_t stats;
	uint32_t package;

	PORTB &= ~(1<<CS);  // Set CS LOW 
	delayMicroseconds(10);

	stats = SPI(address);

	package |= SPI((datagram >> 24) & 0xff);
	package <<= 8;
	package |= SPI((datagram >> 16) & 0xff);
	package <<= 8;
	package |= SPI((datagram >> 8) & 0xff);
	package <<= 8;
	package |= SPI((datagram) & 0xff);

	PORTB |= (1<<CS); // Set CS HIGH

	/* Serial.println(package, HEX);
	Serial.println(stats, BIN);*/

	return package;

}

uint8_t uStepperS::SPI(uint8_t data){

	SPDR = data;

	// Wait for transmission complete
	while(!( SPSR & (1 << SPIF) ));    

	return SPDR;

}

void uStepperS::setup(void){

	
}

void uStepperS::moveSteps( int32_t steps ){

	position = setRegisterValue(XTARGET + WRITE_ACCESS, steps + position);

}




void TIMER1_COMPA_vect(void){

	// pointer->encoder.captureAngle();

}
