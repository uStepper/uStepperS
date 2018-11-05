#include <uStepperS.h>

uStepperS stepper; 

void setup() {

  Serial.begin(9600);
  
  stepper.setMaxAcceleration( 50 ); // Steps/s
  stepper.setup();

}


void loop() {

  /** Set the speed */
  stepper.setRPM( 25 );

  /** Print current speed in RPM */
  Serial.println( stepper.encoder.getSpeed() * 60.0 ) ;

}