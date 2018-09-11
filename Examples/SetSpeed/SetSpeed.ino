#include <uStepperS.h>

uStepperS stepper(100000, 20000); 

void setup() {

  Serial.begin(9600);
  
  stepper.setup();
  
}

int rpm = 25;

void loop() {

  for(uint8_t x = 0; x < 10; x++){
    
    stepper.setRPM( rpm * x );

    delay(2000);
  }
  
  delay(2000);
  
}