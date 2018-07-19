#include <Arduino.h>
/* Pin definition for AEAT-8800-Q24 */

#define CS PB2 /* Chip select - Pin 10 on ATmega328p */

#define DIN PB3 /* Data in - Pin 11 on ATmega328p (MOSI) */

#define DO PB4 /* Data out - Pin 12 on ATmega328p (MISO) */

#define CLK PB5 /* Clock signal - Pin 13 on ATmega328p (SCK) */


class uStepperEncoder
{

	friend class uStepperS;

	public:

		void setup(void);

		float getAngle(void);

		void captureAngle(void);

		uStepperEncoder(void);

	private:
		
		volatile uint16_t angle;

		uint8_t SPI( uint8_t data );
	
};

