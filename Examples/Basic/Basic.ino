// basic motion control via serial port issued commands
// written by grey@christoforo.net
// send 'b' character to begin
// then send "hr\t" or "hl\t" to home clockwise or counterclockwise
// or send "X\t" to ask the motor to move X steps

#include <uStepperS.h>

#define STEPSPERREV 200 //Number of steps pr revolution. 200 for a 1.8deg motor, 400 for a 0.9deg motor
#define RES (STEPSPERREV *256)/360.0//calculate step pr. degree
#define STEPPRMM 53.55//step pr. mm for the rail used in the demo
#define MMPRSTEP 1/STEPPRMM//mm pr. step
#define MMPRDEG MMPRSTEP*RES//mm pr. degree
#define STALLSENSITIVITY 2//sensitivity of the stall detection, between -64 and 63 - higher number is less sensitive

void(* doReset) (void) = 0;  // hardware reset

uStepperS stepper;

// Desired rpm for homing
int16_t rpm = 50;

// steps to move
int32_t nsteps = 0;

// command string from serial port
String cmd;

void setup() {
  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  Serial.println();
  Serial.print("Send b to begin.");
  // trap execution here and wait for begin signal
  // prevents unwanted motion/initialization, especially during programming
  while (Serial.read() != 'b'){
    ;
  }
  Serial.setTimeout(5000); // in ms

  stepper.setup(NORMAL, STEPSPERREV);
  stepper.checkOrientation(30.0);       //Check orientation of motor connector with +/- 30 microsteps movement
  stepper.setMaxAcceleration(2000);//use an acceleration of 2000 fullsteps/s^2
  stepper.setMaxVelocity(500);//Max velocity of 500 fullsteps/s

  Serial.println();
  Serial.println("Supported commands are:");
  Serial.println("hr = home clockwise");
  Serial.println("hl = home counterclockwise");
  Serial.println("integer = move <integer> steps");
  Serial.println("Send TAB character (0x09) to complete command.");
  Serial.print(">>> ");
}

void loop() {
  if(Serial.available())
  {
    cmd = Serial.readStringUntil(0x09);  // read until tab
    nsteps = cmd.toInt();
    if (nsteps == 0) {  // non-integer command seen
      if (cmd == "hl"){
        Serial.print(cmd);
        Serial.print(": Homing clockwise...");
        stepper.moveToEnd(CW, rpm, STALLSENSITIVITY);      //Reset to CW endpoint
      } else if (cmd == "hr"){
        Serial.print(cmd);
        Serial.print(": Homing counterclockwise...");
        stepper.moveToEnd(CCW, rpm, STALLSENSITIVITY);      //Reset to CCW endpoint
      } else if (cmd == "reboot"){
        Serial.print(cmd);
        Serial.print(": Rebooting...");
        Serial.write(0x04);  // end of transmission
        Serial.end();
        doReset();
      } else if (cmd == "hang"){
        Serial.print(cmd);
        Serial.print(": Goodbye.");
        Serial.write(0x04);  // end of transmission
        Serial.end();
        while(true){
          ; // chill right here forever
        }
      } else if (cmd == ""){
        ;  // nop
      } else {
        Serial.print("Command not understood: ");
        Serial.print(cmd);
      }
    } else {  // integer command seen
      Serial.print(cmd);
      Serial.print(": Moving ");
      Serial.print(nsteps);
      Serial.print(" steps...");
      stepper.moveSteps(nsteps);
      while(getMotorState());  // wait for move to complete
    }
    Serial.println();
    Serial.print(">>> ");
  }
}
