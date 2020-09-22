/********************************************************************************************
* 	 	File: 		uStepperEncoder.h														*
*		Version:    2.2.0                                           						*
*      	Date: 		September 22nd, 2020  	                                    			*
*      	Authors: 	Thomas Hørring Olsen                                   					*
*					Emil Jacobsen															*
*                                                   										*	
*********************************************************************************************
*	(C) 2020																				*
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
* @file uStepperEncoder.h
*
* @brief      Function prototypes and definitions for the AEAT8800-Q24 Encoder
*
*             This file contains class and function prototypes for the AEAT8800-Q24 Encoder,
*             as well as necessary constants and global variables.
*
* @author     Thomas Hørring Olsen (thomas@ustepper.com)
*/
#include <Arduino.h>

#define CS PB2 /**< Chip select for AEAT-8800-Q24 - Pin 10 on ATmega328pB */
#define DIN PB3 /**< Data in for AEAT-8800-Q24 - Pin 11 on ATmega328pB (MOSI) */
#define DO PB4 /**< Data out for AEAT-8800-Q24 - Pin 12 on ATmega328pB (MISO) */
#define CLK PB5 /**< Clock signal for AEAT-8800-Q24 - Pin 13 on ATmega328pB (SCK) */

#define ENCODERDATATOSTEP 51200.0/65536.0 /**< Constant to convert raw encoder data to 1/256th steps*/
#define ENCODERDATATOREVOLUTIONS 60.0/65536.0 /**< Constant to convert raw encoder data to revolutions */
#define ANGLETOENCODERDATA 65535.0/360.0 /**< Constant to convert angle to raw encoder data */

/**
 * @brief      Prototype of class for the AEAT8800-Q24 encoder
 *
 *             This class enables the user of the library to access the AEAT8800-Q24
 *             encoder on the uStepper S board.
 */
class uStepperEncoder
{
	public:
		/**
		 * @brief	Constructor of uStepperEncoder class
		 */
		uStepperEncoder(void);

		/**
		 * @brief		Initiation of the encoder
		 *
		 *				This function initiates all the registers of the encoder.
		 *
		 * @param[in]	_pointer - reference to the uStepper S object
		 */
		void init( uStepperS * _pointer );

		/**
		 * @brief      Define new reference(home) position
		 *
		 *             This function redefines the reference position to the current
		 *             angle of the shaft
		 * 
		 * 	@param[in]  initialAngle - Home position offset from zero
		 */
		void setHome( float initialAngle = 0 );

		/**
		 * @brief      Return the current shaft angle in degrees
		 *
		 *             This function returns the last captured shaft angle.
		 *             The resolution of the angle returned by this function is
		 *             0.0054931640625 degrees (16 bits)
		 *
		 * @return     Floating point representation of the current motor shaft
		 *             angle
		 */
		float getAngle( void );

		/**
		 * @brief      	Return the current shaft angle in raw encoder readings
		 *
		 *             	This function returns the last captured shaft angle, as 
		 *				output directly from the encoder.
		 *             	The resolution of the angle returned by this function is
		 *             	16 bits
		 *
		 * @return     	raw 16 bit encoder representation of the current motor shaft
		 *             	angle
		 */
		uint16_t getAngleRaw( void );

		/**
		 * @brief      Returns the angle moved from reference position in degrees
		 *
		 *             This function returns the angle moved from the shaft
		 *             reference position. When the uStepper S is first powered on,
		 *             the reference position is reset to the current shaft
		 *             position, meaning that this function will return the angle
		 *             rotated with respect to the angle the motor initially had. It
		 *             should be noted that this function is absolute to an
		 *             arbitrary number of revolutions !
		 *
		 *             The reference position can be reset at any point in time, by
		 *             use of the setHome() function.
		 * 
		 * @param[in]  filtered - if true, the function returns the filtered angle. if false, the unfiltered angle is returned
		 *
		 * @return     The angle moved in degrees.
		 */
		float getAngleMoved( bool filtered = true);

		/**
		 * @brief      Returns the angle moved from reference position in raw encoder readings
		 *
		 *             This function returns the angle moved from the shaft
		 *             reference position. When the uStepper S is first powered on,
		 *             the reference position is reset to the current shaft
		 *             position, meaning that this function will return the angle
		 *             rotated with respect to the angle the motor initially had. It
		 *             should be noted that this function is absolute to an
		 *             arbitrary number of revolutions !
		 *
		 *             The reference position can be reset at any point in time, by
		 *             use of the setHome() function.
		 * 
		 * @param[in]  filtered - if true, the function returns the filtered angle. if false, the unfiltered angle is returned
		 *
		 * @return     The angle moved in raw encoder readings.
		 */
		int32_t getAngleMovedRaw( bool filtered = true );

		/**
		 * @brief      Measure the current speed of the motor
		 *
		 *             This function returns the current speed of the motor. The
		 *             speed is not calculated in this function, it is merely
		 *             returning a variable. The speed is calculated in the
		 *             interrupt routine associated with timer1.
		 *
		 * @return     Current speed in 1/256th steps per second
		 */
		float getSpeed(void);

		/**
		 * @brief      Measure the current speed of the motor
		 *
		 *             This function returns the current speed of the motor. The
		 *             speed is not calculated in this function, it is merely
		 *             returning a variable. The speed is calculated in the
		 *             interrupt routine associated with timer1.
		 *
		 * @return     Current speed in RPM
		 */
		float getRPM(void);

		/**
		 * @brief      Capture the current shaft angle
		 *
		 *             This function is used by the ISR to read the current angle of the motor shaft.
		 *             The Angle is read by means of the SSI interface, using the hardware SPI port.
		 *
		 * @return 		samples the encoder and returns the current angle reading
		 */
		uint16_t captureAngle( void );

		/**
		 * @brief      	Get encoder status
		 *
		 *				This function reads the current status of the encoder 
		 *
		 * @return 		Current status register content
		 */
		uint8_t getStatus( void );

		/**
		 * @brief      detect magnet
		 *
		 *				This function reads the current status of the encoder with regards to placement
		 *				of the magnet.
		 *
		 * @return 		checks if the magnet is placed in the correct distance from the encoder:
		 *				0: everything is OK
		 *				1: magnet is placed either too far away or too close to the encoder
		 */
		bool detectMagnet(void);

		/** variable used for filtering the encoder readings*/
		volatile int32_t smoothValue;
		
		/** variable used for filtering the encoder speed estimation*/
		volatile float speedSmoothValue;

		/** Angle of the shaft at the reference position. */
		volatile uint16_t encoderOffset;

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


		/** Variable used to store the last measured rotational speed of
		* the motor shaft */
		volatile float curSpeed;

		/** Object to hold speed filter */
		volatile posFilter_t encoderFilter;

		/** Filter constant for encoder feedback **/
		volatile  uint8_t Beta = 5;

	private:
		
		/** Reference to the main object */
		uStepperS * pointer;

		/**
		 * @brief      Set the output level of the chip select pin
		 *
		 * @param[in]  state - 1 = High, 0 = low
		 */
		void chipSelect(bool state);

		/** Status bits from the encoder */
		uint8_t status; 
		int32_t userAngleOffset = 0;	

		volatile int32_t angleMovedRaw = 0;

};

