#include <Arduino.h>
#include <uStepperS.h>
/**  TMC5130 Register Address Defines */

#define GCONF				0x00 	// Global configuration flags

#define DIRECTION(n)		(((n)&0x1)<<4) // Set motor direction
#define EN_PWM_MODE(n)		(((n)&0x1)<<2) // Enable stealthMode
#define I_SCALE_ANALOG(n)	(((n)&0x1)<<0) // Use external current reference

#define GSTAT				0x01 
#define X_COMPARE 			0x05	// Position  comparison  register
#define IHOLD_IRUN			0x10	// Driver current control
#define TPOWERDOWN			0x11
#define TPWMTHRS			0x13
#define TCOOLTHRS			0x14	// This is the lower threshold velocity for switching on smart energy coolStep and stallGuard feature.
#define THIGH				0x15
#define RAMPMODE			0x20	// Driving mode (Velocity, Positioning, Hold)
#define XACTUAL				0x21	// Actual motor position
#define VACTUAL 			0x22	// Actual  motor  velocity  from  ramp  generator

/** Ramp curves */
#define VSTART_REG			0x23	// Motor start velocity
#define A1_REG				0x24	// First  acceleration  between  VSTART  and  V1
#define V1_REG				0x25	// First  acceleration  /  deceleration  phase  target velocity
#define AMAX_REG			0x26	// Second  acceleration  between  V1  and  VMAX
#define VMAX_REG 			0x27	// This is the target velocity in velocity mode. It can be changed any time during a motion.
#define DMAX_REG			0x28	// Deceleration between VMAX and V1
#define D1_REG				0x2A 	// Deceleration  between  V1  and  VSTOP
#define VSTOP_REG			0x2B	// Motor stop velocity (unsigned)
#define TZEROWAIT			0x2C	// Defines  the  waiting  time  after  ramping  down
#define XTARGET				0x2D	// Target position for ramp mode
#define VDCMIN				0x33
#define SW_MODE 			0x34	// Switch mode configuration
#define RAMP_STAT			0x35	// Ramp status and switch event status
#define XLATCH				0x36	// Latches  XACTUAL  upon  a programmable switch event


#define PWMCONF				0x70

#define FREEWHEEL(n)		(((n)&0x3UL)<<20)
#define PWM_AUTOSCALE(n)	(((n)&0x1UL)<<18)
#define PWM_FREQ(n)			(((n)&0x3UL)<<16)
#define PWM_GRAD(n)			(((n)&0xFF)<<8)
#define PWM_AMPL(n)			(((n)&0xFF)<<0)


/** Chopper and driver configuration */

#define CHOPCONF		0x6C	

#define DISS2G(n)		(((n)&0x1UL)<<30)
#define DEDGE(n)		(((n)&0x1UL)<<29)
#define INTPOL(n)		(((n)&0x1UL)<<28)
#define MRES(n)			(((n)&0xFUL)<<24)
#define SYNC(n)			(((n)&0xFUL)<<20)
#define VHIGHCHM(n)		(((n)&0x1UL)<<19)
#define VHIGHFS(n)		(((n)&0x1UL)<<18)
#define VSENSE(n)		(((n)&0x1UL)<<17)
#define TBL(n)			(((n)&0x3UL)<<15)
#define CHM(n)			(((n)&0x1UL)<<14)
#define RNDTF(n)		(((n)&0x1)<<13)
#define DISFDCC(n)		(((n)&0x1)<<12)
#define TFD3(n)			(((n)&0x1)<<11)
#define HEND(n)			(((n)&0xF)<<7)
#define HSTRT_TFD(n)	(((n)&0x7)<<4) 
#define TOFF(n)			(((n)&0xF)<<0)


/* CoolStep smart current control register and stallGuard2 configuration **/

#define COOLCONF		0x6D	
#define SFILT(n)		(((n)&0x1UL)<<24)
#define SGT(n)			(((n)&0x7FUL)<<16)
#define SEIMIN(n)		(((n)&0x1UL)<<15)
#define SEDN(n)			(((n)&0x3)<<13)
#define SEMAX(n)		(((n)&0xF)<<8)
#define SEUP(n)			(((n)&0x3)<<5)
#define SEMIN(n)		(((n)&0xF)<<0) 


#define DCCTRL			0x6E
#define DC_SG(n)		(((n)&0xFFUL)<<16)
#define DC_TIME(n)		(((n)&0x3FF)<<0)


#define DRV_STATUS 		0x6F	// stallGuard2 value and driver error flags

#define IHOLDDELAY(n)	(((n)&0xFUL)<<16)
#define IRUN(n)			(((n)&0x1F)<<8)
#define IHOLD(n)		(((n)&0x1F)<<0)


#define WRITE_ACCESS	0x80	// Write access for spi communication

/** Modes for RAMPMODE register */

#define POSITIONING_MODE	0x00		// using all A, D and V parameters)
#define VELOCITY_MODE_POS	0x01		// positiv VMAX, using AMAX acceleration
#define VELOCITY_MODE_NEG	0x02		// negativ VMAX, using AMAX acceleration
#define HOLD_MODE			0x03		// velocity remains unchanged, unless stop event occurs

class uStepperDriver{

friend class uStepperS;

	public:

		uStepperDriver( void );

		void init( uStepperS * _pointer );

		void setPosition( int32_t position );

		void setVelocity( uint32_t velocity );

		void setAcceleration( uint16_t acceleration );

		void setDeceleration( uint16_t deceleration );

		void setRampProfile( uint32_t velocity, uint16_t acceleration, uint16_t deceleration );

		void setRampMode( uint8_t mode );

		void setCurrent( uint8_t current );

		void setHoldCurrent( uint8_t current );

		void updateCurrent( void );

		void setDirection( bool direction );

		void setShaftDirection( bool direction );


		void stop( void );

		void reset( void );

		void enableStealth( void);


		int32_t getVelocity( void );

		int32_t getPosition( void );

		bool positionReached( void );

		/** Status bits from the driver */
		uint8_t status; 

	private:

		uStepperS * pointer; 	

		uint8_t current = 16;
		uint8_t holdCurrent = 16;
		uint8_t holdDelay = 0;

		/** Default acceleration profile for positioning mode */
		uint32_t VSTART = 0;
		uint32_t V1 	= 100000;
		uint32_t VMAX	= 200000;
		uint32_t VSTOP 	= 10;
		uint16_t A1 	= 1000;
		uint16_t AMAX	= 400;
		uint16_t DMAX	= 800;
		uint16_t D1 	= 2000;

		int32_t writeRegister( uint8_t address, uint32_t datagram );

		int32_t readRegister( uint8_t address );

		void chipSelect(bool state);

};
