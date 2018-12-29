/*/*
  Name:    SetSpeed.ino
  Created: 29-12-2018
  Author:  SolidGeek
  Description: This example shows how to use the Serial monitor to set the motor speed in RPM
*/

#include <uStepperS.h>

uStepperS stepper; 

void setup() {

  Serial.begin(115200);

  stepper.setup();
  
  stepper.setMaxVelocity( 500 );    // Steps/s
  stepper.setMaxAcceleration( 10 ); // Steps/s
  stepper.setHoldCurrent(0);        // Enabling free-wheeling when RPM = 0
  stepper.setRPM(0);

}


void loop() {

  if( Serial.available() > 0){

    uint32_t velocity = Serial.readStringUntil('\n').toInt();
    stepper.setRPM( velocity );
    
  }
}