/********************************************************************************************
* 	 	File: 		uStepperDriver.h														*
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
* @file uStepperDriver.h
*
* @brief      Function prototypes and definitions for the TMC5130 motor driver
*
*             This file contains class and function prototypes for the TMC5130 motor driver,
*             as well as necessary constants and global variables.
*
* @author     Thomas Hørring Olsen (thomas@ustepper.com)
*/
#include <Arduino.h>
#include <uStepperS.h>

/*  TMC5130 Register Address Defines */

#define GCONF				0x00 	/**< Please check datasheet for register description */

#define DIRECTION(n)		(((n)&0x1)<<4) /**< Please check datasheet for register description */
#define EN_PWM_MODE(n)		(((n)&0x1)<<2) /**< Please check datasheet for register description */
#define I_SCALE_ANALOG(n)	(((n)&0x1)<<0) /**< Please check datasheet for register description */

#define GSTAT				0x01 	/**< Please check datasheet for register description */
#define X_COMPARE 			0x05	/**< Please check datasheet for register description */
#define IHOLD_IRUN			0x10	/**< Please check datasheet for register description */
#define TPOWERDOWN			0x11	/**< Please check datasheet for register description */
#define TSTEP				0x12
#define TPWMTHRS			0x13	/**< Please check datasheet for register description */
#define TCOOLTHRS			0x14	/**< Please check datasheet for register description */
#define THIGH				0x15	/**< Please check datasheet for register description */
#define RAMPMODE			0x20	/**< Please check datasheet for register description */
#define XACTUAL				0x21	/**< Please check datasheet for register description */
#define VACTUAL 			0x22	/**< Please check datasheet for register description */

/** Ramp curves */
#define VSTART_REG			0x23	/**< Please check datasheet for register description */
#define A1_REG				0x24	/**< Please check datasheet for register description */
#define V1_REG				0x25	/**< Please check datasheet for register description */
#define AMAX_REG			0x26	/**< Please check datasheet for register description */
#define VMAX_REG 			0x27	/**< Please check datasheet for register description */
#define DMAX_REG			0x28	/**< Please check datasheet for register description */
#define D1_REG				0x2A 	/**< Please check datasheet for register description */
#define VSTOP_REG			0x2B	/**< Please check datasheet for register description */
#define TZEROWAIT			0x2C	/**< Please check datasheet for register description */
#define XTARGET				0x2D	/**< Please check datasheet for register description */
#define VDCMIN				0x33	/**< Please check datasheet for register description */
#define SW_MODE 			0x34	/**< Please check datasheet for register description */
#define SG_STOP(n)			(((n)&0x1)<<10)	/**< Please check datasheet for register description */
#define RAMP_STAT			0x35	/**< Please check datasheet for register description */
#define XLATCH				0x36	/**< Please check datasheet for register description */


#define PWMCONF				0x70	/**< Please check datasheet for register description */

#define FREEWHEEL(n)		(((n)&0x3UL)<<20)	/**< Please check datasheet for register description */
#define PWM_AUTOSCALE(n)	(((n)&0x1UL)<<18)	/**< Please check datasheet for register description */
#define PWM_FREQ(n)			(((n)&0x3UL)<<16)	/**< Please check datasheet for register description */
#define PWM_GRAD(n)			(((n)&0xFF)<<8)		/**< Please check datasheet for register description */
#define PWM_AMPL(n)			(((n)&0xFF)<<0)		/**< Please check datasheet for register description */


/** Chopper and driver configuration */

#define CHOPCONF		0x6C					/**< Please check datasheet for register description */

#define DISS2G(n)		(((n)&0x1UL)<<30)		/**< Please check datasheet for register description */
#define DEDGE(n)		(((n)&0x1UL)<<29)		/**< Please check datasheet for register description */
#define INTPOL(n)		(((n)&0x1UL)<<28)	/**< Please check datasheet for register description */
#define MRES(n)			(((n)&0xFUL)<<24)	/**< Please check datasheet for register description */
#define SYNC(n)			(((n)&0xFUL)<<20)	/**< Please check datasheet for register description */
#define VHIGHCHM(n)		(((n)&0x1UL)<<19)	/**< Please check datasheet for register description */
#define VHIGHFS(n)		(((n)&0x1UL)<<18)	/**< Please check datasheet for register description */
#define VSENSE(n)		(((n)&0x1UL)<<17)	/**< Please check datasheet for register description */
#define TBL(n)			(((n)&0x3UL)<<15)	/**< Please check datasheet for register description */
#define CHM(n)			(((n)&0x1UL)<<14)	/**< Please check datasheet for register description */
#define RNDTF(n)		(((n)&0x1)<<13)	/**< Please check datasheet for register description */
#define DISFDCC(n)		(((n)&0x1)<<12)	/**< Please check datasheet for register description */
#define TFD3(n)			(((n)&0x1)<<11)	/**< Please check datasheet for register description */
#define HEND(n)			(((n)&0xF)<<7)	/**< Please check datasheet for register description */
#define HSTRT_TFD(n)	(((n)&0x7)<<4) 	/**< Please check datasheet for register description */
#define TOFF(n)			(((n)&0xF)<<0)	/**< Please check datasheet for register description */


/* CoolStep smart current control register and stallGuard2 configuration **/

#define COOLCONF		0x6D	/**< Please check datasheet for register description */
#define SFILT(n)		(((n)&0x1UL)<<24)	/**< Please check datasheet for register description */
#define SGT(n)			(((n)&0x7FUL)<<16)	/**< Please check datasheet for register description */
#define SEIMIN(n)		(((n)&0x1UL)<<15)	/**< Please check datasheet for register description */
#define SEDN(n)			(((n)&0x3)<<13)	/**< Please check datasheet for register description */
#define SEMAX(n)		(((n)&0xF)<<8)	/**< Please check datasheet for register description */
#define SEUP(n)			(((n)&0x3)<<5)	/**< Please check datasheet for register description */
#define SEMIN(n)		(((n)&0xF)<<0) 	/**< Please check datasheet for register description */


#define DCCTRL			0x6E	/**< Please check datasheet for register description */
#define DC_SG(n)		(((n)&0xFFUL)<<16)	/**< Please check datasheet for register description */
#define DC_TIME(n)		(((n)&0x3FF)<<0)	/**< Please check datasheet for register description */

#define DRV_STATUS 		0x6F	/**< stallGuard2 value and driver error flags*/

#define IHOLDDELAY(n)	(((n)&0xFUL)<<16)	/**< Please check datasheet for register description */
#define IRUN(n)			(((n)&0x1F)<<8)	/**< Please check datasheet for register description */
#define IHOLD(n)		(((n)&0x1F)<<0)	/**< Please check datasheet for register description */


#define WRITE_ACCESS	0x80	/**< Write access for spi communication*/

/** Modes for RAMPMODE register */

#define POSITIONING_MODE	0x00		/**< using all A, D and V parameters)*/
#define VELOCITY_MODE_POS	0x01		/**< positiv VMAX, using AMAX acceleration*/
#define VELOCITY_MODE_NEG	0x02		/**< negativ VMAX, using AMAX acceleration*/
#define HOLD_MODE			0x03		/**< velocity remains unchanged, unless stop event occurs*/

#define DRIVER_STOP 0					/**< Define label for indicating driver is in standstill mode */
#define DRIVER_VELOCITY 1	/**< Define label for indicating driver is in velocity mode */
#define DRIVER_POSITION 2	/**< Define label for indicating driver is in position mode */

#define ACCELERATIONCONVERSION 1.0/116.415321827	/**< page 74 datasheet*/
#define VELOCITYCONVERSION 1.0/0.953674316	/**< page 74 datasheet*/

/**
 * @brief      Prototype of class for the TMC5130 Driver
 *
 *             This class enables the user of the library to access the TMC5130
 *             Driver on the uStepper S board.
 */
class uStepperDriver{

friend class uStepperS;
	public:
		/**
		 * @brief      Constructor
		 *
		 *             This is the constructor of the uStepperDriver class.
		 */
		uStepperDriver( void );

		/**
		 * @brief		Initiation of the motor driver
		 *
		 *				This function initiates all the registers of the motor driver.
		 *
		 * @param[in]	_pointer - reference to the uStepper S object
		 */
		void init( uStepperS * _pointer );

		/**
		 * @brief		Set the motor position
		 *
		 *				This function tells the motor to go to an absolute position.
		 *
		 * @param[in]	position - position the motor should move to, in micro steps (1/256th default)
		 */
		void setPosition( int32_t position );

		/**
		 * @brief		Set motor velocity
		 *
		 *				This function tells the motor driver to make the motor spin at a specific velocity.
		 *				input unit is in ((microsteps/s)/8MHz)/2^23. See page 74 of datasheet for more information 
		 *
		 * @param[in]	velocity - see description for unit
		 */
		void setVelocity( uint32_t velocity );
		/**
		 * @brief		Set motor acceleration
		 *
		 *				This function tells the motor driver to use a specific acceleration while ramping up the velocity.
		 *				input unit is in (microsteps/s^2)/116.42. See page 74 of datasheet for more information 
		 *
		 * @param[in]	acceleration - see description for unit
		 */
		void setAcceleration( uint32_t acceleration );
		/**
		 * @brief		Set motor deceleration
		 *
		 *				This function tells the motor driver to use a specific deceleration while ramping down the velocity.
		 *				input unit is in (microsteps/s^2)/116.42. See page 74 of datasheet for more information 
		 *
		 * @param[in]	deceleration - see description for unit
		 */
		void setDeceleration( uint32_t deceleration );

		/**
		 * @brief		Set motor driver current
		 *
		 * @param[in]	current - sets the current to use during movements in percent (0-100)
		 */
		void setCurrent( uint8_t current );
		
		/**
		 * @brief		Set motor driver hold current
		 *
		 * @param[in]	current - sets the current to use during standstill in percent (0-100)
		 */
		void setHoldCurrent( uint8_t current );

		/**
		 * @brief		Set motor driver direction
		 *
		 *				This function is used to set the direction of the motor driver
		 *				to either normal or inverted. In normal mode, a positiv target
		 *				position corresponds to a CW movement, while a negative target
		 *				position corresponds to a CCW movement.
		 *
		 * @param[in]	direction - 0 = normal direction, 1 = inverted direction
		 */
		void setShaftDirection( bool direction );

		/**
		 * @brief		Stops any ongoing movement with deceleration
		 */
		void stop( void );

		/**
		 * @brief		Returns the current speed of the motor driver
		 *
		 *				This function returns the current speed of the 
		 *				internal ramp generator of the motor driver.
		 *				unit is in ((microsteps/s)/8MHz)/2^23. 
		 *				See page 74 of datasheet for more information 
		 *
		 * @return		see description for unit
		 */
		int32_t getVelocity( void );

		/**
		 * @brief		Returns the current position of the motor driver
		 *
		 *				This function returns the position of the motor
		 *				drivers internal position counter.
		 *				unit is in microsteps (default 1/256th). 
		 *
		 * @return		microsteps (default 1/256th).
		 */
		int32_t getPosition( void );

		/**
		 * @brief		Resets the internal position counter of the motor driver
		 * 
		 * @param[in]  initialSteps - Home step offset from zero
		 */
		void setHome(int32_t initialSteps = 0);
		
		/**
		 * @brief		Write a register of the motor driver
		 *
		 *				This function is used to write a value into
		 *				a register in the TMC5130 motor driver. Please
		 *				refer to datasheet for details.
		 *				
		 *				When using this function you are on your own and expect you know what you are doing !
		 *
		 * @return 		Return data associated with last SPI command
		 *
		 * @param[in]	address - Register to write
		 *
		 * @param[in]	datagram - data to write into the register
		 */
		int32_t writeRegister( uint8_t address, uint32_t datagram );

		/**
		 * @brief		Reads a register from the motor driver
		 *
		 *				This function is used to read the content of
		 *				a register in the TMC5130 motor driver. Please
		 *				refer to datasheet for details.
		 *
		 * @return 		Return data of the read register
		 *
		 * @param[in]	address - Register to read
		 */
		int32_t readRegister( uint8_t address );

		/**
		 * @brief		Returns the load measurement used for Stall detection
		 */
		uint16_t getStallValue( void );

		/** target position in microsteps*/
		volatile int32_t xTarget = 0;

		/** current position in microsteps*/
		volatile int32_t xActual = 0;


	protected:
		/** Status bits from the driver */
		uint8_t status; 

		/** STOP, VELOCITY, POSITION*/
		uint8_t mode = DRIVER_STOP;

		uStepperS * pointer; 	

		uint8_t current = 16;
		uint8_t holdCurrent = 0;
		uint8_t holdDelay = 0;

		/** Default acceleration profile for positioning mode */
		uint32_t VSTART = 0;
		uint32_t V1 	= 0;
		uint32_t VMAX	= 200000;
		uint32_t VSTOP 	= 10;
		uint16_t A1 	= 600;
		uint16_t AMAX	= 100;
		uint16_t DMAX	= 600;
		uint16_t D1 	= 600;


		void chipSelect(bool state);

		/**
		 * @brief		Writes the current setting registers of the motor driver  
		 */
		void updateCurrent( void );

		/**
		 * @brief		Set motor driver to position mode or velocity mode
		 *
		 * @param[in]	mode - can be either POSITIONING_MODE or VELOCITY_MODE_POS
		 */
		void setRampMode( uint8_t mode );

		void setDirection( bool direction );

		void reset( void );

		void enableStealth( void );

		void enableStallguard( int8_t threshold, bool stopOnStall, float rpm );

		void disableStallguard( void );

		void clearStall( void );

		void readMotorStatus(void);



};
