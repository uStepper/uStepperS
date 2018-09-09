#include <uStepperS.h>

uStepperDriver::uStepperDriver( uStepperS * p){

	this->pointer = p;

}

void uStepperDriver::setup(uint8_t ihold, uint8_t irun ){

	pointer->chipSelect(CS_DRIVER, true);

	writeRegister(IHOLD_IRUN, ( IHOLD(ihold) | IRUN(irun) | IHOLDDELAY(7) ) );

	/* Resets GCONF for TMC5130 and enable stealth */
	// writeRegister(GCONF, 0x04); 

	writeRegister(GCONF, 0x00); 


	/* Set CHOPCONF for TMC5130 */
	writeRegister(CHOPCONF, (TOFF(5) | HSTRT_TFD(4) | HEND(2) | VHIGHCHM(1) | VHIGHFS(1) ) );

	writeRegister( VDCMIN, 50000 ); // Enable dcStep up to 20000 steps / t
	/* Resets PWMCONF for TMC5130 */
	// writeRegister(PWMCONF, 0x000401C8); 
	writeRegister(PWMCONF, 0x00); 

	setDriverProfile(0);

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

			writeRegister(THIGH, 	200000);

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

	uint8_t stats;
	uint32_t package;

	address += WRITE_ACCESS;

	pointer->chipSelect(CS_DRIVER, false);

	stats = pointer->SPI(address);

	package |= pointer->SPI((datagram >> 24) & 0xff);
	package <<= 8;

	package |= pointer->SPI((datagram >> 16) & 0xff);
	package <<= 8;

	package |= pointer->SPI((datagram >> 8) & 0xff);
	package <<= 8;

	package |= pointer->SPI((datagram) & 0xff);

	pointer->chipSelect(CS_DRIVER, true); // Set CS HIGH

	/* Serial.println(package, HEX);
	Serial.println(stats, BIN);*/

	return package;

}


void uStepperDriver::setSpeed(uint32_t velocity){

	writeRegister(VMAX, velocity);

}

int32_t uStepperDriver::setPosition( int32_t position ){

	return writeRegister(XTARGET, position);

}
