#include <uStepperS.h>

uStepperServo servo;    //instantiate uStepperServo object
uint16_t value;

void setup()
{
  servo.attach(A1);     //Attach Servo to A1
  Serial.begin(9600);
}

void loop()
{
  if(Serial.available())
  {
    value = Serial.parseInt();      //Read angle argument from serial
    servo.write(value);             //Write angle to servo object
  }

  uStepperServo::refresh();         //Call this method at least once every 50ms to keep the servo running
}
