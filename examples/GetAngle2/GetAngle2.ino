#include <uStepperS.h>

uStepperS stepper; 

void setup() {

  Serial.begin(9600);
  stepper.setup();
  
}


void loop() {
  
  Serial.println( stepper.encoder->getAngle() );

}
