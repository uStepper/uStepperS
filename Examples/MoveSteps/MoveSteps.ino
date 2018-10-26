#include <uStepperS.h>

uStepperS stepper; 

void setup() {

  Serial.begin(9600);

  stepper.setMaxVelocity( 20 ); // Steps/s
  stepper.setMaxAcceleration( 20 ); // Steps/s
  
  stepper.setup();


}

void loop() {

  stepper.driver.moveSteps( 5000 );
  
  Serial.println( stepper.driver.getSpeed() );

  Serial.println( stepper.encoder.getAngle() );
  
  delay(1000); 
  
}