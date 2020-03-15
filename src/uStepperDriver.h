/********************************************************************************************
* 	 	File: 		uStepperDriver.h														*
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

#define GCONF				0x00 	/**< Global configuration flags */

#define DIRECTION(n)		(((n)&0x1)<<4) /**< Set motor direction*/
#define EN_PWM_MODE(n)		(((n)&0x1)<<2) /**< Enable stealthMode*/
#define I_SCALE_ANALOG(n)	(((n)&0x1)<<0) /**< Use external current reference*/

#define GSTAT				0x01 	/**< DESCRIPTION PENDING */
#define X_COMPARE 			0x05	/**< Position  comparison  register*/
#define IHOLD_IRUN			0x10	/**< Driver current control*/
#define TPOWERDOWN			0x11	/**< DESCRIPTION PENDING */
#define TSTEP				0x12
#define TPWMTHRS			0x13	/**< DESCRIPTION PENDING */
#define TCOOLTHRS			0x14	/**< This is the lower threshold velocity for switching on smart energy coolStep and stallGuard feature.*/
#define THIGH				0x15	/**< DESCRIPTION PENDING */
#define RAMPMODE			0x20	/**< Driving mode (Velocity, Positioning, Hold)*/
#define XACTUAL				0x21	/**< Actual motor position*/
#define VACTUAL 			0x22	/**< Actual  motor  velocity  from  ramp  generator*/

/** Ramp curves */
#define VSTART_REG			0x23	/**< Motor start velocity*/
#define A1_REG				0x24	/**< First  acceleration  between  VSTART  and  V1*/
#define V1_REG				0x25	/**< First  acceleration  /  deceleration  phase  target velocity*/
#define AMAX_REG			0x26	/**< Second  acceleration  between  V1  and  VMAX*/
#define VMAX_REG 			0x27	/**< This is the target velocity in velocity mode. It can be changed any time during a motion.*/
#define DMAX_REG			0x28	/**< Deceleration between VMAX and V1*/
#define D1_REG				0x2A 	/**< Deceleration  between  V1  and  VSTOP*/
#define VSTOP_REG			0x2B	/**< Motor stop velocity (unsigned)*/
#define TZEROWAIT			0x2C	/**< Defines  the  waiting  time  after  ramping  down*/
#define XTARGET				0x2D	/**< Target position for ramp mode*/
#define VDCMIN				0x33	/**< DESCRIPTION PENDING */
#define SW_MODE 			0x34	/**< Switch mode configuration*/
#define SG_STOP(n)			(((n)&0x1)<<10)	/**< DESCRIPTION PENDING */
#define RAMP_STAT			0x35	/**< Ramp status and switch event status*/
#define XLATCH				0x36	/**< Latches  XACTUAL  upon  a programmable switch event*/


#define PWMCONF				0x70	/**< DESCRIPTION PENDING */

#define FREEWHEEL(n)		(((n)&0x3UL)<<20)	/**< DESCRIPTION PENDING */
#define PWM_AUTOSCALE(n)	(((n)&0x1UL)<<18)	/**< DESCRIPTION PENDING */
#define PWM_FREQ(n)			(((n)&0x3UL)<<16)	/**< DESCRIPTION PENDING */
#define PWM_GRAD(n)			(((n)&0xFF)<<8)		/**< DESCRIPTION PENDING */
#define PWM_AMPL(n)			(((n)&0xFF)<<0)		/**< DESCRIPTION PENDING */


/** Chopper and driver configuration */

#define CHOPCONF		0x6C					/**< DESCRIPTION PENDING */

#define DISS2G(n)		(((n)&0x1UL)<<30)		/**< DESCRIPTION PENDING */
#define DEDGE(n)		(((n)&0x1UL)<<29)		/**< DESCRIPTION PENDING */
#define INTPOL(n)		(((n)&0x1UL)<<28)	/**< DESCRIPTION PENDING */
#define MRES(n)			(((n)&0xFUL)<<24)	/**< DESCRIPTION PENDING */
#define SYNC(n)			(((n)&0xFUL)<<20)	/**< DESCRIPTION PENDING */
#define VHIGHCHM(n)		(((n)&0x1UL)<<19)	/**< DESCRIPTION PENDING */
#define VHIGHFS(n)		(((n)&0x1UL)<<18)	/**< DESCRIPTION PENDING */
#define VSENSE(n)		(((n)&0x1UL)<<17)	/**< DESCRIPTION PENDING */
#define TBL(n)			(((n)&0x3UL)<<15)	/**< DESCRIPTION PENDING */
#define CHM(n)			(((n)&0x1UL)<<14)	/**< DESCRIPTION PENDING */
#define RNDTF(n)		(((n)&0x1)<<13)	/**< DESCRIPTION PENDING */
#define DISFDCC(n)		(((n)&0x1)<<12)	/**< DESCRIPTION PENDING */
#define TFD3(n)			(((n)&0x1)<<11)	/**< DESCRIPTION PENDING */
#define HEND(n)			(((n)&0xF)<<7)	/**< DESCRIPTION PENDING */
#define HSTRT_TFD(n)	(((n)&0x7)<<4) 	/**< DESCRIPTION PENDING */
#define TOFF(n)			(((n)&0xF)<<0)	/**< DESCRIPTION PENDING */


/* CoolStep smart current control register and stallGuard2 configuration **/

#define COOLCONF		0x6D	/**< DESCRIPTION PENDING */
#define SFILT(n)		(((n)&0x1UL)<<24)	/**< DESCRIPTION PENDING */
#define SGT(n)			(((n)&0x7FUL)<<16)	/**< DESCRIPTION PENDING */
#define SEIMIN(n)		(((n)&0x1UL)<<15)	/**< DESCRIPTION PENDING */
#define SEDN(n)			(((n)&0x3)<<13)	/**< DESCRIPTION PENDING */
#define SEMAX(n)		(((n)&0xF)<<8)	/**< DESCRIPTION PENDING */
#define SEUP(n)			(((n)&0x3)<<5)	/**< DESCRIPTION PENDING */
#define SEMIN(n)		(((n)&0xF)<<0) 	/**< DESCRIPTION PENDING */


#define DCCTRL			0x6E	/**< DESCRIPTION PENDING */
#define DC_SG(n)		(((n)&0xFFUL)<<16)	/**< DESCRIPTION PENDING */
#define DC_TIME(n)		(((n)&0x3FF)<<0)	/**< DESCRIPTION PENDING */

#define DRV_STATUS 		0x6F	/**< stallGuard2 value and driver error flags*/

#define IHOLDDELAY(n)	(((n)&0xFUL)<<16)	/**< DESCRIPTION PENDING */
#define IRUN(n)			(((n)&0x1F)<<8)	/**< DESCRIPTION PENDING */
#define IHOLD(n)		(((n)&0x1F)<<0)	/**< DESCRIPTION PENDING */


#define WRITE_ACCESS	0x80	/**< Write access for spi communication*/

/** Modes for RAMPMODE register */

#define POSITIONING_MODE	0x00		/**< using all A, D and V parameters)*/
#define VELOCITY_MODE_POS	0x01		/**< positiv VMAX, using AMAX acceleration*/
#define VELOCITY_MODE_NEG	0x02		/**< negativ VMAX, using AMAX acceleration*/
#define HOLD_MODE			0x03		/**< velocity remains unchanged, unless stop event occurs*/

#define DRIVER_STOP 0					/**< DESCRIPTION PENDING */
#define DRIVER_VELOCITY 1	/**< DESCRIPTION PENDING */
#define DRIVER_POSITION 2	/**< DESCRIPTION PENDING */

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
		 */
		void setHome(void);
		
		/**
		 * @brief		Write a register of the motor driver
		 *
		 *				This function is used to write a value into
		 *				a register in the TMC5130 motor driver. Please
		 *				refer to datasheet for details.
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

		uint16_t getStallValue( void );

		void clearStall( void );

		bool isStalled( void );

		/** target position in microsteps*/
		volatile int32_t xTarget = 0;

	protected:
		/** Status bits from the driver */
		uint8_t status; 

		/** STOP, VELOCITY, POSITION*/
		uint8_t mode = DRIVER_STOP;

		uStepperS * pointer; 	

		uint8_t current = 16;
		uint8_t holdCurrent = 10;
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

		void enableStealth( void);

		void enableStallguard( bool stopOnStall, int8_t threshold );

		void disableStallguard( void );

		void readMotorStatus(void);



};
