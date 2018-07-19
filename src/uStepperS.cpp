#include <uStepperS.h>

uStepperS * pointer;

uStepperS::uStepperS(void)
{

	this->encoder.setup();

	pointer = this;

}


void uStepperS::setup(void){

	
}

void TIMER1_COMPA_vect(void){

	pointer->encoder.captureAngle();

}
