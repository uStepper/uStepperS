#include <Arduino.h>
/** Chip select for AEAT-8800-Q24 - Pin 10 on ATmega328p */
#define CS PB2 

/** Data in for AEAT-8800-Q24 - Pin 11 on ATmega328p (MOSI) */
#define DIN PB3 

/** Data out for AEAT-8800-Q24 - Pin 12 on ATmega328p (MISO) */
#define DO PB4 

/** Clock signal for AEAT-8800-Q24 - Pin 13 on ATmega328p (SCK) */
#define CLK PB5 

#define ENCODERDATATOSTEP 51200.0/65536.0
#define ENCODERDATATOREVOLUTIONS 60.0/65536.0

class uStepperEncoder
{
	public:

		/**
		 * @brief	Constructor of uStepperEncoder class
		 */
		uStepperEncoder(void);

		void init( uStepperS * _pointer );

		void setHome( void );

		/**
		 * @brief      Return the current shaft angle
		 *
		 *             This function returns the last captured shaft angle.
		 *             The resolution of the angle returned by this function is
		 *             0.0054931640625 degrees (16 bits)
		 *
		 * @return     Floating point representation of the current motor shaft
		 *             angle
		 */
		float getAngle( void );

		uint16_t getAngleRaw( void );

		float getAngleMoved( void );

		int32_t getAngleMovedRaw( void );


		/**
		 * @brief      Measure the current speed of the motor
		 *
		 *             This function returns the current speed of the motor. The
		 *             speed is not calculated in this function, it is merely
		 *             returning a variable. The speed is calculated in the
		 *             interrupt routine associated with timer1.
		 *
		 * @return     Current speed in revolutions per second (RPS)
		 */
		float getSpeed(void);

		float getRPM(void);


		/**
		 * @brief      Capture the current shaft angle
		 *
		 *             This function is used by the ISR to read the current angle of the motor shaft.
		 *             The Angle is read by means of the SSI interface, using the hardware SPI port.
		 */
		uint16_t captureAngle( void );

		uint8_t getStatus( void );

		bool detectMagnet(void);


		/** Angle of the shaft at the reference position. */
		uint16_t encoderOffset;

		/** This variable always contain the current rotor angle, relative
		  * to a single revolution */
		volatile uint16_t angle;

		/** Variable used to store that measured angle moved from the
		  * reference position */
		volatile int32_t angleMoved;


		/** Used to stored the previous measured angle for the speed
		  * measurement, and the calculation of angle moved from reference
		  * position */
		volatile uint16_t oldAngle;


		/** This variable contains the number of revolutions in either
		  * direction, since last home position was set. negative numbers
		  * corresponds to CCW turns, and positive to CW turns */
		volatile int16_t revolutions;


		/** Variable used to store the last measured rotational speed of
		* the motor shaft */
		volatile float curSpeed;

		volatile uint32_t deltaTime;
		volatile posFilter_t encoderFilter;

	private:
		
		/* Reference to the main object */
		uStepperS * pointer;

		void chipSelect(bool state);

		/** Status bits from the encoder */
		uint8_t status; 

		

};

