/*
*			DropIn example sketch!
*
*	Pin connections:
*	-----------------------------
*	| Controller | uStepper S    |
*	|----------------------------|
*	|	Enable   |		D2		 |
*	|	Step     |		D3		 |
*	|	Dir      |		D4		 |
*	|	GND      |		GND		 |
*	------------------------------	
*/

#include <uStepperS.h>
uStepperS stepper;

void setup() {

  Serial.begin(9600);
  stepper.setup(DROPIN, 200, 4.1, 2.0, 1.0);
}

void loop() {
  Serial.println(stepper.encoder.getAngleMoved());
}
