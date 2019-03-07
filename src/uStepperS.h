/********************************************************************************************
* 	 	File: 		uStepperS.h 																*
*		Version:    1.0.0                                         						*
*      	date: 		July 12th, 2018 	                                    				*
*      	Author: 	Emil Jacobsen 		                                  					*
*                                                   										*	
*********************************************************************************************
*	(C) 2018																				*
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
 * @brief	Function prototypes and definitions for the uStepperS library
 *
 *			This file contains class and function prototypes for the library,
 *			as well as necessary constants and global variables.
 *
 * @author	Emil Jacobsen (emil@ustepper.com)
 */

#ifndef _USTEPPER_S_H_
#define _USTEPPER_S_H_

#ifndef __AVR_ATmega328PB__
	#error !!This library only supports the ATmega328PB MCU!!
#endif

#define CW 1
#define CCW 0

typedef struct 
{
	float posError = 0.0;
	float posEst = 0.0;				//<--- Filtered Position
	float velIntegrator = 0.0;		//<--- Filtered velocity
	float velEst = 0.0;
}posFilter_t;

#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>
class uStepperS;
#include <uStepperEncoder.h>
#include <uStepperDriver.h>




#define DRV_ENN PD4 
#define SD_MODE PD5
#define SPI_MODE PD6

#define CS_DRIVER PE2
#define CS_ENCODER PD7 

#define MOSI1 PE3
#define MOSI_ENC PC2
#define MISO1 PC0  
#define SCK1 PC1 

/** Value defining normal mode*/	
#define NORMAL 	0	
/** Value defining dropin mode for 3d printer/CNC controller boards*/
#define DROPIN 	1						
/** Value defining PID mode for normal library functions*/
#define PID 	2	

#define CLOCKFREQ 16000000.0

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
 * @brief	Measures angle of motor.
 *
 *			This interrupt routine is in charge of sampling the encoder
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

class uStepperS
{

friend class uStepperDriver;
friend class uStepperEncoder;
friend void interrupt0(void);
friend void TIMER1_COMPA_vect(void) __attribute__ ((signal,used));
public:			
	/** Instantiate object for the encoder */

    // uStepperEncoder *encoder;

	/** Instantiate object for the driver */
	uStepperDriver driver;
	
	/** Instantiate object for the driver */
	uStepperEncoder encoder;

	/**
	 * @brief	Constructor of uStepper class
	 */
	uStepperS();

	uStepperS(float acceleration, float velocity);

	/**
	 * @brief	Internal function to prepare the uStepperS in the constructor
	 */
	void init( void );

	/**
	 * @brief      Initializes the different parts of the uStepper object
	 *
	 *             This function initializes the different parts of the uStepper
	 *             object, and should be called in the setup() function of the
	 *             arduino sketch. This function is needed as some things, like
	 *             the timer can not be setup in the constructor, since arduino
	 *             for some strange reason, resets a lot of the AVR registers
	 *             just before entering the setup() function.
	 *
	 * @param[in]  mode             	Default is normal mode. Pass the constant
	 *                              	"DROPIN" to configure the uStepper to act as
	 *                              	dropin compatible to the stepstick. Pass the
	 *                              	constant "PID", to enable PID feature for
	 *                              	regular movement functions, such as
	 *                              	moveSteps()
	 * @param[in]  stepsPerRevolution   Number of fullsteps per revolution
	 *
	 * @param[in]  pTerm            	The proportional coefficent of the PID
	 *                              	controller
	 * @param[in]  iTerm            	The integral coefficent of the PID
	 *                              	controller
	 * @param[in]  dterm            	The differential coefficent of the PID
	 *                              	controller
	 * @param[in]  dropinStepSize		number of steps per fullstep, send from
	 *									external dropin controller   
	 * @param[in]  setHome          	When set to true, the encoder position is
	 *									Reset. When set to false, the encoder
	 *									position is not reset.
	 */
	void setup(	uint8_t mode = NORMAL,
				uint16_t stepsPerRevolution = 200, 
				float pTerm = 0.75, 
				float iTerm = 3.0, 
				float dTerm = 0.0,
				uint16_t dropinStepSize = 16,
				bool setHome = true);	


	/**
	 * @brief      Set the velocity in rpm
	 *
	 *             This function lets the user set the velocity of the motor in rpm. 
	 *             A negative value switches direction of the motor.
	 *
	 * @param      rpm  - The velocity in rotations per minute
	 */
	void setRPM( float rpm );

	/**
	 * @brief      Set the maximum acceleration of the stepper motor.
	 *
	 *             This function lets the user set the max acceleration used 
	 *             by the stepper driver.
	 *
	 * @param      acceleration  - Maximum acceleration in steps/s^2
	 */
	void setMaxAcceleration	( float acceleration );

	/**
	 * @brief      Set the maximum deceleration of the stepper motor.
	 *
	 *             This function lets the user set the max deceleration used 
	 *             by the stepper driver.
	 *
	 * @param      deceleration  - Maximum deceleration in steps/s^2
	 */
	void setMaxDeceleration ( float deceleration );

	/**
	 * @brief      Set the maximum velocity of the stepper motor.
	 *
	 *             This function lets the user set the max velocity used 
	 *             by the stepper driver.
	 *
	 * @param      velocity  - Maximum velocity in steps/s^2
	 */
	void setMaxVelocity	( float velocity );

	/**
	 * @brief      Set motor output current.
	 *
	 *             This function allows the user to change the current setting of the motor driver.
	 *
	 * @param      current  - Desired current in percent (0% - 100%)
	 */
	void setCurrent( double current );

	/**
	 * @brief      Set motor hold current.
	 *
	 *             This function allows the user to change the current setting of the motor driver.
	 *
	 * @param      current  - Desired hold current in percent (0% - 100%)
	 */
	void setHoldCurrent( double current );

	/**
	 * @brief      Make the motor perform a predefined number of steps
	 *
	 *             This function makes the motor perform a predefined number of
	 *             steps, using the acceleration profile. The motor will accelerate
	 *             at the rate set by setMaximumAcceleration(), and eventually reach the speed set
	 *             by setMaximumVelocity() function. The direction of rotation
	 *             is set by the argument "dir".
	 *
	 * @param      steps     -	Number of steps to be performed.
	 */
	void moveSteps( int32_t steps );

	/**
	 * @brief      	Moves the motor to a relative angle
	 *
	 * @param  	    angle     -	Relative angle from current position (negative value allowed)
	 */
	void moveAngle( float angle );

	/**
	 * @brief      	Moves the motor to an absolute angle
	 *
	 * @param   	angle     -	Absolute angle (negative value allowed)
	 */
	void moveToAngle( float angle );

	/**
	 * @brief      Make the motor rotate continuously
	 *
	 *             This function makes the motor rotate continuously, using the
	 *             acceleration profile.
	 *
	 * @param      dir   - Can be set to "CCW" or "CW" (without the quotes)
	 */
	void runContinous( bool dir );

	/**
	 * @brief      Get the angle moved from reference position
	 *
	 * @return     The angle moved.
	 */
	float angleMoved( void );

	bool uStepperS::getMotorState(void);

	void stop( void );

	bool isStalled(void);

	void brakeMotor(bool brake);

	void enablePid(void);
	void disablePid(void);

	float moveToEnd(bool dir);


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

	uint8_t SPI( uint8_t data );

	void setSPIMode( uint8_t mode );

	void chipSelect( uint8_t pin , bool state );

	void filterSpeedPos(posFilter_t *filter, int32_t steps);

	float pid(float error, bool reset = 0);
};



#endif