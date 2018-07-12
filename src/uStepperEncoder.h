class uStepperEncoder
{

	public:

		volatile int32_t angleMoved;		
		volatile uint16_t angle;				 	

		uStepperEncoder(void);
		
		float getAngle(void);
	
};

class spiMaster
{

	public:

		SPI(void);

		void begin(void);

		uint8_t readByte(void);

}
