#include <uStepperS.h>

uStepperS stepper(100000, 20000); 

void setup() {

  Serial.begin(9600);
  
  stepper.setup();
  
}

void loop() {
  
  /*
  for(uint8_t x = 0; x < 40; x++){
    stepper.setRPM(5*x);
    delay(1000);
  }
  */

  stepper.driver->setSpeed( 200000 );

  delay(5000);
  
}
