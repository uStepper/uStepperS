#include <uStepperS.h>

uStepperS stepper; 

void setup() {

  stepper.setup();
  Serial.begin(9600);

}

void loop() {

  Serial.println( stepper.encoder.getAngle() );

  delay(10);
  
}