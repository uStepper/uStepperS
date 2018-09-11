#include <uStepperS.h>

uStepperS stepper(100000, 20000); 

void setup() {

  Serial.begin(9600);
  
  stepper.setup();
  
}

void loop() {
  
  stepper.setRPM( 100 );

  delay(5000);
  
}