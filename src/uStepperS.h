/********************************************************************************************
* 	 	File: 		uStepperS.h 															*
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
* @file uStepperS.h
*
* @brief      Function prototypes and definitions for the uStepper S library
*
*             This file contains class and function prototypes for the library,
*             as well as necessary constants and global variables.
*
* @author     Thomas Hørring Olsen (thomas@ustepper.com)
*
*	\mainpage Arduino library for the uStepper S Board
*	
*	This is the uStepper S Arduino library, providing software functions for the different features of the uStepper S board.
*
*	\par Features
*	The uStepper S library contains the following features:
*
*	- Dropin feature for applications like 3D printers
*	- Closed loop PID position controller
*	- Control of RC servo motors
*	- Measure the current position of the shaft (absolute, multiple revolutions)
*	- Measure the current speed of the motor 
*	- Stall detection for use in e.g. limit detection functionality 
*	
*	The library uses timer one in order to function properly, meaning that unless the user of this library
*	can accept the loss of some functionality, this timer is unavailable and the registers associated with these timers
*	should not be reconfigured.
*
*	Timer one is used for sampling the encoder in order to provide the ability to keep track of both the current speed and the
*	angle moved since the board was reset (or a new home position was configured). Also the drop-in features missed step detection and 
*	correction is done in this timer. 
*	
*	\par EEPROM Usage information
*	\warning
*	\warning Please be aware that the uStepper uses the EEPROM to store settings related to the Dropin application.
*	\warning If you are not using this, then this has no impact for your application, and you can ignore this section !
*	\warning
*	\warning EEPROM address 0 to 15 contains the different settings for dropin. If your application uses the EEPROM,
*	\warning Please use another location than these !
*
*	\par Installation
*	To install the uStepper S library into the Arduino IDE, perform the following steps:
*
*	- Go to Sketch->Include Libraries->Manage Libraries... in the arduino IDE
*	- Search for "uStepper S", in the top right corner of the "Library Manager" window
*	- Install uStepper S library 
*	
*	The library is tested with Arduino IDE 1.8.8
*	
*	\warning MAC users should be aware, that OSX does NOT include SILABS VCP drivers, needed to upload sketches to the uStepper S, by default. This driver should be 
*	downloaded and installed from SILABS's website:
*	\warning https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers
*	\warning             The uStepper S should NOT be connected to the USB port while installing this driver !
*	\warning This is not (commonly) a problem for windows/linux users, as these drivers are most often already included in the OS
*
*	\par Copyright
*
*	(C)2019 uStepper ApS	
*																	
*	www.ustepper.com 																	
*
*	administration@ustepper.com 														
*																							
*	<img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" />																
*
*	The code contained in this file is released under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License</a>	
*																							
*	The code in this library is provided without warranty of any kind - use at own risk!		
* 	neither uStepper ApS nor the author, can be held responsible for any damage		
* 	caused by the use of the code contained in this library ! 	
*
*	\par To do list
*	- Clean out in unused variables
*	- Update comments
*
*	\par Known Bugs
*	- does not properly release motor in dropin mode
*
*	\author Thomas Hørring Olsen (thomas@ustepper.com)
*	\par Change Log
*	\version 1.0.1:
*	- Fixed bug in functions to set acceleration and deceleration
*	- moved a couple of functions in uStepperDriver.h from public to protected section of class
*	- added documentation
*	\version 1.0.0:
*	- Bug fixes
*	- New Dropin PID code
*	- Added dropin CLI interface
*	- Fixed stall detection, and added user sensitivity parameter
*	\version 0.1.1:
*	- Bug fixes
*	\version 0.1.0:	
*	- Initial release
*	
*/

#ifndef _USTEPPER_S_H_
#define _USTEPPER_S_H_

#ifndef __AVR_ATmega328PB__
	#error !!This library only supports the ATmega328PB MCU!!
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <inttypes.h>
#include <uStepperServo.h>
#define CW 1	/**< DESCRIPTION PENDING */
#define CCW 0	/**< DESCRIPTION PENDING */

#define POSITION_REACHED 0x20	/**< DESCRIPTION PENDING */
#define VELOCITY_REACHED 0x10	/**< DESCRIPTION PENDING */
#define STANDSTILL 0x08	/**< DESCRIPTION PENDING */
#define STALLGUARD2 0x04	/**< DESCRIPTION PENDING */

/**
 * @brief      Union to easily split a float into its binary representation
 * 
 */
typedef union
{
	float f;			/**< normal float value*/
	uint8_t bytes[4];	/**< binary representation, split into an array of 4 bytes*/
}floatBytes_t;

/**
 * @brief      	Struct to store dropin settings
 *
 *				This struct contains the current dropin settings, aswell as a checksum,
 *				which is used upon loading of settings from EEPROM, to determine if the 
 *				settings in the EEPROM are valid.								
 * 
 */
typedef struct 
{
	floatBytes_t P;				/**< Proportional gain of the dropin PID controller	*/
	floatBytes_t I;				/**< Integral gain of the dropin PID controller	*/			
	floatBytes_t D;				/**< Differential gain of the dropin PID controller	*/
	uint8_t invert;				/**< Inversion of the "direction" input in dropin mode. 0 = NOT invert, 1 = invert	*/
	uint8_t holdCurrent;		/**< Current to use when the motor is NOT rotating. 0-100 %	*/
	uint8_t runCurrent;			/**< Current to use when the motor is rotating. 0-100 %	*/
	uint8_t checksum;			/**< Checksum	*/
}dropinCliSettings_t;

/**
 * @brief      	Struct for encoder velocity estimator
 *
 *				This struct contains the variables for the velocity estimator.
 */
typedef struct 
{
	float posError = 0.0;			/**< Position estimation error*/
	float posEst = 0.0;				/**< Position Estimation (Filtered Position)*/
	float velIntegrator = 0.0;		/**< Velocity integrator output (Filtered velocity)*/
	float velEst = 0.0;				/**< Estimated Velocity*/
}posFilter_t;


class uStepperS;
#include <uStepperEncoder.h>
#include <uStepperDriver.h>

#define HARD 0	/**< DESCRIPTION PENDING */
#define SOFT 1	/**< DESCRIPTION PENDING */

#define DRV_ENN PD4 	/**< DESCRIPTION PENDING */
#define SD_MODE PD5	/**< DESCRIPTION PENDING */
#define SPI_MODE PD6	/**< DESCRIPTION PENDING */

#define CS_DRIVER PE2	/**< DESCRIPTION PENDING */
#define CS_ENCODER PD7 	/**< DESCRIPTION PENDING */

#define MOSI1 PE3	/**< DESCRIPTION PENDING */
#define MOSI_ENC PC2	/**< DESCRIPTION PENDING */
#define MISO1 PC0  	/**< DESCRIPTION PENDING */
#define SCK1 PC1 	/**< DESCRIPTION PENDING */

/** Value defining normal mode*/	
#define NORMAL 	0		
/** Value defining dropin mode for 3d printer/CNC controller boards*/
#define DROPIN 	1						
/** Value defining PID mode for normal library functions*/
#define PID 	2	

#define CLOCKFREQ 16000000.0	/**< DESCRIPTION PENDING */

/** Frequency at which the encoder is sampled, for keeping track of angle moved and current speed */
#define ENCODERINTFREQ 1000.0	
/** Frequency at which the encoder is sampled, for keeping track of angle moved and current speed */
#define ENCODERINTPERIOD 1.0/ENCODERINTFREQ		
/** Constant to convert angle difference between two interrupts to speed in revolutions per second. Dividing by 10 as each speed is calculated from 10 samples */
#define ENCODERSPEEDCONSTANT ENCODERINTFREQ/65535.0	
/**	P term in the PI filter estimating the step rate of incomming pulsetrain in DROPIN mode*/
#define PULSEFILTERKP 120.0
/**	I term in the PI filter estimating the step rate of incomming pulsetrain in DROPIN mode*/
#define PULSEFILTERKI 1900.0*ENCODERINTPERIOD

/**
 * @brief	Interrupt routine for critical tasks.
 *
 *			This interrupt routine is in charge of sampling the encoder, process the data and handle PID
 */
extern "C" void TIMER1_COMPA_vect(void) __attribute__ ((signal,used));

/**
 * @brief      Used by dropin feature to take in step pulses
 *
 *             This interrupt routine is used by the dropin feature to keep
 *             track of step and direction pulses from main controller
 */
void interrupt0(void);

/**
 * @brief      Used by dropin feature to take in enable signal
 *
 *             This interrupt routine is used by the dropin feature to keep
 *             track of enable signal from main controller
 */
void interrupt1(void);


/**
 * @brief      Prototype of class for accessing all features of the uStepper S in
 *             a single object.
 *
 *             This class enables the user of the library to access all features
 *             of the uStepper S board, by use of a single object.
 */
class uStepperS
{

friend class uStepperDriver;
friend class uStepperEncoder;
friend void interrupt0(void);
friend void TIMER1_COMPA_vect(void) __attribute__ ((signal,used));
public:			

	/** Instantiate object for the driver */
	uStepperDriver driver;
	
	/** Instantiate object for the Encoder */
	uStepperEncoder encoder;

	/**
	 * @brief	Constructor of uStepper class
	 */
	uStepperS();

	/**
	 * @brief	Overloaded Constructor of uStepper class
	 */
	uStepperS(float acceleration, float velocity);

	/**
	 * @brief	Internal function to prepare the uStepperS in the constructor
	 */
	void init( void );

	/**
	 * @brief      Initializes the different parts of the uStepper S object
	 *
	 *             This function initializes the different parts of the uStepper S
	 *             object, and should be called in the setup() function of the
	 *             arduino sketch. This function is needed as some things, like
	 *             the timer can not be setup in the constructor, since arduino
	 *             for some strange reason, resets a lot of the AVR registers
	 *             just before entering the setup() function.
	 *
	 * @param[in]  mode             	Default is normal mode. Pass the constant
	 *                              	"DROPIN" to configure the uStepper to act as
	 *                              	dropin compatible to the stepstick. Pass the
	 *                              	constant "PID", to enable closed loop feature for
	 *                              	regular movement functions, such as
	 *                              	moveSteps()
	 * @param[in]  stepsPerRevolution   Number of fullsteps per revolution
	 *
	 * @param[in]  pTerm            	The proportional coefficent of the DROPIN PID
	 *                              	controller
	 * @param[in]  iTerm            	The integral coefficent of the DROPIN PID
	 *                              	controller
	 * @param[in]  dTerm            	The differential coefficent of the DROPIN PID
	 *                              	controller
	 * @param[in]  dropinStepSize		number of steps per fullstep, send from
	 *									external dropin controller   
	 * @param[in]  setHome          	When set to true, the encoder position is
	 *									Reset. When set to false, the encoder
	 *									position is not reset.
	 * @param[in]  invert           	Inverts the motor direction for dropin
	 *									feature. 0 = NOT invert, 1 = invert.
	 *									this has no effect for other modes than dropin
	 * @param[in]  runCurrent       	Sets the current (in percent) to use while motor is running.
	 * @param[in]  holdCurrent      	Sets the current (in percent) to use while motor is NOT running
	 */
	void setup(	uint8_t mode = NORMAL,
				uint16_t stepsPerRevolution = 200, 
				float pTerm = 10.0, 
				float iTerm = 0.0, 
				float dTerm = 0.0,
				uint16_t dropinStepSize = 16,
				bool setHome = true,
				uint8_t invert = 0,
				uint8_t runCurrent = 50,
				uint8_t holdCurrent = 30);	


	/**
	 * @brief      Set the velocity in rpm
	 *
	 *             This function lets the user set the velocity of the motor in rpm. 
	 *             A negative value switches direction of the motor.
	 *
	 * @param[in]  rpm  - The velocity in rotations per minute
	 */
	void setRPM( float rpm );

	/**
	 * @brief      Set the maximum acceleration of the stepper motor.
	 *
	 *             This function lets the user set the max acceleration used 
	 *             by the stepper driver.
	 *
	 * @param[in]      acceleration  - Maximum acceleration in steps/s^2
	 */
	void setMaxAcceleration	( float acceleration );

	/**
	 * @brief      Set the maximum deceleration of the stepper motor.
	 *
	 *             This function lets the user set the max deceleration used 
	 *             by the stepper driver.
	 *
	 * @param[in]      deceleration  - Maximum deceleration in steps/s^2
	 */
	void setMaxDeceleration ( float deceleration );

	/**
	 * @brief      Set the maximum velocity of the stepper motor.
	 *
	 *             This function lets the user set the max velocity used 
	 *             by the stepper driver.
	 *
	 * @param[in]      velocity  - Maximum velocity in steps/s
	 */
	void setMaxVelocity	( float velocity );

	/**
	 * @brief      Set motor output current.
	 *
	 *             This function allows the user to change the current setting of the motor driver.
	 *
	 * @param[in]      current  - Desired current in percent (0% - 100%)
	 */
	void setCurrent( double current );

	/**
	 * @brief      Set motor hold current.
	 *
	 *             This function allows the user to change the current setting of the motor driver.
	 *
	 * @param[in]      current  - Desired hold current in percent (0% - 100%)
	 */
	void setHoldCurrent( double current );

	/**
	 * @brief      Make the motor perform a predefined number of steps
	 *
	 *             This function makes the motor perform a predefined number of
	 *             steps, using the acceleration profile. The motor will accelerate
	 *             at the rate set by setMaxAcceleration(), decelerate at the rate set by setMaxDeceleration() and eventually reach the speed set
	 *             by setMaxVelocity() function. The direction of rotation
	 *             is set by the sign of the commanded steps to perform
	 *
	 * @param[in]      steps     -	Number of steps to be performed. an input value of
	 *								300 makes the motor go 300 steps in CW direction, and
	 *								an input value of -300 makes the motor move 300 steps
	 *								in CCW direction.
	 */
	void moveSteps( int32_t steps );

	/**
	 * @brief      	Moves the motor rotate a specific angle relative to the current position
	 *
	 *              This function makes the motor a rotate by a specific angle relative to 
	 *			    the current position, using the acceleration profile. The motor will accelerate
	 *              at the rate set by setMaxAcceleration(), decelerate at the rate set by 
	 *				setMaxDeceleration() and eventually reach the speed set
	 *              by setMaxVelocity() function. The direction of rotation
	 *              is set by the sign of the commanded angle to move
	 *
	 * @param[in]  	    angle     -	Angle to move. an input value of
	 *								300 makes the motor go 300 degrees in CW direction, and
	 *								an input value of -300 makes the motor move 300 degrees
	 *								in CCW direction.
	 */
	void moveAngle( float angle );

	/**
	 * @brief      	Moves the motor rotate a specific angle relative to the current position
	 *
	 *              This function makes the motor a rotate by a specific angle relative to 
	 *			    the current position, using the acceleration profile. The motor will accelerate
	 *              at the rate set by setMaxAcceleration(), decelerate at the rate set by 
	 *				setMaxDeceleration() and eventually reach the speed set
	 *              by setMaxVelocity() function. The direction of rotation
	 *              is set by the sign of the commanded angle to move
	 *
	 * @param[in]  	    angle     -	Angle to move. an input value of
	 *								300 makes the motor go 300 degrees in CW direction, and
	 *								an input value of -300 makes the motor move 300 degrees
	 *								in CCW direction.
	 */
	void moveToAngle( float angle );

	/**
	 * @brief      Make the motor rotate continuously
	 *
	 *             This function makes the motor rotate continuously, using the
	 *             acceleration profile.
	 *
	 * @param[in]      dir   - Can be set to "CCW" or "CW" (without the quotes)
	 */
	void runContinous( bool dir );

	/**
	 * @brief      Get the angle moved from reference position in degrees
	 *
	 * @return     The angle moved in degrees.
	 */
	float angleMoved( void );

	/**
	 * @brief      Get the current motor driver state
	 *
	 *				This function is used to check some internal status flags of the driver.
	 *				The argument is used to specify the flag to check
	 *
	 *	param[in]	statusType - status flag to check. Possible values:
	 *					POSITION_REACHED - has last commanded position been reached?
	 *					VELOCITY_REACHED - has last commanded velocity been reached?
	 *					STANDSTILL - Are the motor currently stopped?
	 *					STALLGUARD2 - Has the stallguard been trickered?
	 *					
	 *
	 * @return     The angle moved.
	 */
	bool uStepperS::getMotorState(uint8_t statusType = POSITION_REACHED);

	/**
	 * @brief      Stop the motor
	 *
	 *             	This function stops any ongoing motor movement. The "mode" argument
	 *				determines whether the motor should stop with or without
	 *				a deceleration phase
	 *
	 * @param      mode  -	can be set to "HARD" for no deceleration phase
	 *						or "SOFT" for deceleration phase.
	 */
	void stop( bool mode = HARD );

	/**
	 * @brief      	This method returns a bool variable indicating wether the motor
	 *				is stalled or not
	 *
	 * @param[in]  	stallSensitivity - Sensitivity of stall detection (0.0 - 1.0), low is more sensitive
	 *
	 * @return     	0 = not stalled, 1 = stalled
	 */
	bool isStalled(float stallSensitivity = 0.992);

	void brakeMotor(bool brake);

	/**
	 * @brief      	This method enables the PID after being disabled  (disablePid).
	 *
	 */
	void enablePid(void);

	/**
	 * @brief      	This method disables the PID until calling enablePid.
	 *
	 */
	void disablePid(void);

	/**
	 * @brief      	Moves the motor to its physical limit, without limit switch
	 *
	 *              This function, makes the motor run continously, untill the
	 *				encoder detects a stall, at which point the motor is assumed
	 *				to be at it's limit.
	 *
	 * @param[in]  	dir  Direction to search for limit
	 *
	 * @param[in]  	stallSensitivity  Sensitivity of stall detection (0.0 - 1.0), low is more sensitive
	 *
	 * @return 		Degrees turned from calling the function, till end was reached
	 */
	float moveToEnd(bool dir, float stallSensitivity = 0.992);

	/**
	 * @brief      This method returns the current PID error
	 * @return     PID error (float)
	 */	
	float getPidError(void);

	/**
	 * @brief      	This method is used to change the PID proportional parameter P.
	 *
	 * @param[in]  	P - PID proportional part P
	 *
	 */
	void setProportional(float P);

	/**
	 * @brief      	This method is used to change the PID integral parameter I.
	 *
	 * @param[in]  	I - PID integral part I
	 *
	 */
	void setIntegral(float I);

	/**
	 * @brief      	This method is used to change the PID differential parameter D.
	 *
	 * @param[in]  	D - PID differential part D
	 *
	 */
	void setDifferential(float D);

	/**
	 * @brief      	This method is used to invert the drop-in direction pin interpretation.
	 *
	 * @param[in]  	invert - 0 = not inverted, 1 = inverted
	 *
	 */
	void invertDropinDir(bool invert);

	/**
	 * @brief      	This method is used to tune Drop-in parameters.
	 *				After tuning uStepper S, the parameters are saved in EEPROM
	 *				
	 * 				Usage:
	 *				Set Proportional constant: 'P=10.002;'
	 *				Set Integral constant: 'I=10.002;'
	 *				Set Differential constant: 'D=10.002;'
	 *				Invert Direction: 'invert;'
	 *				Get Current PID Error: 'error;'
	 *				Get Run/Hold Current Settings: 'current;'
	 *				Set Run Current (percent): 'runCurrent=50.0;'
	 *				Set Hold Current (percent): 'holdCurrent=50.0;'	
	 *
	 */	
	void dropinCli();

	/**
	 * @brief      	This method is used for the dropinCli to take in user commands.
	 *
	 * @param[in]  	cmd - input from terminal for dropinCli
	 *			
	 */
	void parseCommand(String *cmd);
	
	/**
	 * @brief      	This method is used to print the dropinCli menu explainer:
	 *				
	 * 				Usage:
	 *				Show this command list: 'help;'
	 *				Get PID Parameters: 'parameters;'
	 *				Set Proportional constant: 'P=10.002;'
	 *				Set Integral constant: 'I=10.002;'
	 *				Set Differential constant: 'D=10.002;'
	 *				Invert Direction: 'invert;'
	 *				Get Current PID Error: 'error;'
	 *				Get Run/Hold Current Settings: 'current;'
	 *				Set Run Current (percent): 'runCurrent=50.0;'
	 *				Set Hold Current (percent): 'holdCurrent=50.0;'	
	 *
	 */	
	void dropinPrintHelp();
	
private: 

	/** This variable contains the maximum velocity in steps/s, the motor is
	 * allowed to reach at any given point. The user of the library can
	 * set this by use of the setMaxVelocity()
	 */
	float maxVelocity;					

	/** This variable contains the maximum acceleration in steps/s to be used. The
	 * can be set and read by the user of the library using the
	 * functions setMaxAcceleration()
	 */
	float maxAcceleration;
	float maxDeceleration;
	bool invertPidDropinDirection;
	float rpmToVelocity;
	float angleToStep;

	uint16_t microSteps;
	uint16_t fullSteps;
	uint16_t dropinStepSize;

	int32_t stepCnt;

	float stepsPerSecondToRPM;
	float RPMToStepsPerSecond;

	volatile posFilter_t externalStepInputFilter;

	float currentPidSpeed;
	/** This variable is used to indicate which mode the uStepper is
	* running in (Normal, dropin or pid)*/
	uint8_t mode;	
	float pTerm;	
	/** This variable contains the integral coefficient used by the PID */
	float iTerm;		

	float dTerm;
	bool brake;
	volatile bool pidDisabled;
	/** This variable holds information on wether the motor is stalled or not.
	0 = OK, 1 = stalled */
	volatile bool stall;
	// SPI functions

	volatile int32_t pidPositionStepsIssued = 0;
	volatile float currentPidError;
	float stallSensitivity = 0.992;
	uint8_t SPI( uint8_t data );

	void setSPIMode( uint8_t mode );

	void chipSelect( uint8_t pin , bool state );

	void filterSpeedPos(posFilter_t *filter, int32_t steps);

	float pid(float error);
	bool detectStall(int32_t stepsMoved);
	dropinCliSettings_t dropinSettings;
	bool loadDropinSettings(void);
	void saveDropinSettings(void);
	uint8_t dropinSettingsCalcChecksum(dropinCliSettings_t *settings);
};



#endif