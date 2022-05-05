/**
 * @brief
 * Fading: 
 *   This example shows how to fade an LED using the analogWrite() 
 *   function with a uStepperS board.
 * The circuit: 
 *   LED attached from I/O pin D3 to ground.
 * Description:
 *   The uStepperS board uses the ATmega328PB low-power CMOS 8-bit microcontroller.
 *   In order to do PWM at I/O pin D3 a special setup is needed.
 *   I/O pin D3 uses TIMER3B and in order to do that PORTD2 must also be set 
 *   in order to enable the output. See page 159 in the ATmega328PB documentation.
*/

int ledPin = 3;    // LED connected to digital pin 3

void setup() 
{
  // nothing happens in setup
    Serial.begin(9600); 
    delay(100);
    Serial.println("uStepperS fading: ver 20220406");
    Serial.println("Using port D3.");
    Serial.println("Enter at value between 1-255.");
    PORTD |= (1 << 2); // Set PORTD2 in order to enable PWM at D3
    pinMode(ledPin, OUTPUT);
}

void loop() 
{
  int16_t value;
  if(Serial.available())
  {
    value = Serial.parseInt(); //Read PWM value from input
    Serial.print("Setting PWM to: "); Serial.println(value);  
    analogWrite(ledPin, value);
    while(Serial.available()){Serial.read();} // Clear buffer.
  }
}
