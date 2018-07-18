#include <uStepperS.h>

uStepperS stepper; 

void setup() {
  // put your setup code here, to run once:
  stepper.setup();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println( stepper.encoder.getAngle() );

  delay(100);
}
