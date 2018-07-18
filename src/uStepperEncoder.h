#include <Arduino.h>

const uint8_t CS  = PB2; // Pin 10
const uint8_t DIN = PB3; // Pin 11 (MOSI)
const uint8_t DO  = PB4; // Pin 12 (MISO)
const uint8_t CLK = PB5; // Pin 13

class uStepperEncoder
{

	public:

		volatile uint16_t angle;				 	

		uStepperEncoder(void);
		
		float getAngle(void);

		uint8_t SPI( uint8_t data );
	
};

