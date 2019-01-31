#include <uStepperS.h>

uStepperS stepper;
float angle = 360.0;
void setup() {
  // put your setup code here, to run once:
  stepper.setup();
  stepper.setMaxAcceleration(2000);
  stepper.setMaxVelocity(500);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!stepper.getMotorState())
  {
    delay(1000);
    stepper.moveAngle(angle);
    angle = -angle;
  }
   Serial.print("Angle: ");
   Serial.print(stepper.encoder.getAngleMoved());
   Serial.println(" Degrees");
}
