#include <uStepperS.h>

uStepperS stepper; 

void setup() {

  Serial.begin(9600);

  stepper.setMaxVelocity( 100 ); // Steps/s
  stepper.setMaxAcceleration( 50 ); // Steps/s
  
  stepper.setup();

  stepper.driver.stop();

}


unsigned long timer = 0;

void loop() {

  Serial.println(stepper.encoder.getAngle() ) ;

  if( millis() > timer + 3000 ){
    stepper.moveAngle(90);
    timer = millis();
  }

}