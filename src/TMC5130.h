/**  TMC5130 Register Address Defines */

#define GCONF				0x00 	// Global configuration flags
#define X_COMPARE 			0x05	// Position  comparison  register
#define IHOLD_IRUN			0x10	// Driver current control
#define TCOOLTHRS			0x14	// This is the lower threshold velocity for switching on smart energy coolStep and stallGuard feature.
#define RAMPMODE			0x20	// Driving mode (Velocity, Positioning, Hold)
#define XACTUAL				0x21	// Actual motor position
#define VACTUAL 			0x22	// Actual  motor  velocity  from  ramp  generator

/** Ramp curves */
#define VSTART				0x23	// Motor start velocity
#define A_1					0x24	// First  acceleration  between  VSTART  and  V1
#define V_1					0x25	// First  acceleration  /  deceleration  phase  target velocity
#define AMAX				0x26	// Second  acceleration  between  V1  and  VMAX
#define VMAX 				0x27	// This is the target velocity in velocity mode. It can be changed any time during a motion.
#define DMAX				0x28	// Deceleration between VMAX and V1
#define D_1					0x2A 	// Deceleration  between  V1  and  VSTOP
#define VSTOP				0x2B	// Motor stop velocity (unsigned)
#define TZEROWAIT			0x2C	// Defines  the  waiting  time  after  ramping  down
#define XTARGET				0x2D	// Target position for ramp mode
#define SW_MODE 			0x34	// Switch mode configuration
#define RAMP_STAT			0x35	// Ramp status and switch event status
#define XLATCH				0x36	// Latches  XACTUAL  upon  a programmable switch event


/** Chopper and driver configuration */

#define CHOPCONF		0x6C	

#define DISS2G(n)		(((n)&0x1)<<30)
#define DEDGE(n)		(((n)&0x1)<<29)
#define INTPOL(n)		(((n)&0x1)<<28)
#define MRES(n)			(((n)&0xF)<<24)
#define SYNC(n)			(((n)&0xF)<<20)
#define VHIGHCHM(n)		(((n)&0x1)<<19)
#define VHIGHFS(n)		(((n)&0x1)<<18)
#define VSENSE(n)		(((n)&0x1)<<17)
#define TBL(n)			(((n)&0x3)<<15)
#define CHM(n)			(((n)&0x1)<<14)
#define RNDTF(n)		(((n)&0x1)<<13)
#define DISFDCC(n)		(((n)&0x1)<<12)
#define TFD3(n)			(((n)&0x1)<<11)
#define HEND(n)			(((n)&0xF)<<7)
#define HSTRT_TFD(n)	(((n)&0x7)<<4) 
#define TOFF(n)			(((n)&0xF)<<0)


/* CoolStep smart current control register and stallGuard2 configuration **/

#define COOLCONF		0x6D	
#define SFILT(n)		(((n)&0x1)<<24)
#define SGT(n)			(((n)&0x7F)<<16)
#define SEIMIN(n)		(((n)&0x1)<<15)
#define SEDN(n)			(((n)&0x3)<<13)
#define SEMAX(n)		(((n)&0xF)<<8)
#define SEUP(n)			(((n)&0x3)<<5)
#define SEMIN(n)		(((n)&0xF)<<0) 

#define DRV_STATUS 		0x6F	// stallGuard2 value and driver error flags

#define IHOLDDELAY(n)	(((n)&0xF)<<16)
#define IRUN(n)			(((n)&0x1F)<<8)
#define IHOLD(n)		(((n)&0x1F)<<0)


#define WRITE_ACCESS	0x80	// Write access for spi communication

/** Modes for RAMPMODE register */

#define POSITIONING_MODE	0x00		// using all A, D and V parameters)
#define VELOCITY_MODE_POS	0x01		// positiv VMAX, using AMAX acceleration
#define VELOCITY_MODE_NEG	0x02		// negativ VMAX, using AMAX acceleration
#define HOLD_MODE			0x03		// velocity remains unchanged, unless stop event occurs