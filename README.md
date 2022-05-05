# uStepper S

The library contains support for driving the stepper, reading out encoder data. A few examples are included to show the functionality of the library.
The library is supported and tested with in Arduino IDE 1.8.11.

For more information, visit www.ustepper.com

## !! EEPROM Usage information !!

Please be aware that the uStepper uses the EEPROM to store settings related to the Dropin application.
If you are not using this, then this has no impact for your application, and you can ignore this section !

EEPROM address 0 to 15 contains the different settings for dropin. If your application uses the EEPROM,
Please use another location than these !

## Installation

Installation is split into two parts - Hardware and Library. Both are required to use the uStepper S boards.

### Hardware Installation 

### Instructions for users with the hardware for the original uStepper (the one before S and S lite) installed

To add hardware support for uStepper in the Arduino IDE (1.8.8+) do the following:
 - Open Arduino
 - Go to "Tools->Board->Boards Manager..."
 - Search for "uStepper"
 - Choose the "uStepper" board and press "Remove" button
 - close the Boards Manager
 - Go to "File->preferences"
 - Almost at the bottom there is a field stating: "Additional Boards Manager URLs" replace your currently inserted uStepper URL this URL: https://raw.githubusercontent.com/uStepper/uStepperHardware/master/package_ustepper_index.json
 - Press OK
 - Go to "Tools->Board->Boards Manager..."
 - Go to the bottom (after it has loaded new files) select "uStepper by ON Development IVS" and press install

### Instructions for users new to uStepper

To add hardware support for uStepper in the Arduino IDE (1.8.8+) do the following:
 - Open Arduino
 - Go to "File->preferences"
 - Almost at the bottom there is a field stating: "Additional Boards Manager URLs" insert this url: https://raw.githubusercontent.com/uStepper/uStepperHardware/master/package_ustepper_index.json
 - Press OK
 - Go to "Tools->Board->Boards Manager..."
 - Go to the bottom (after it has loaded new files) select "uStepper by ON Development IVS" and press install

You have now added uStepper hardware support and should be able to select uStepper under tools -> boards.

### Library Installation

To add the uStepper S library do the following:
- Open Arduino IDE (Version 1.8.8 or above)
- Go to "Sketch->Include Library->Manage Libraries..."
- Search for "uStepper S"
- Select "uStepper S" and press install
- Close Library Manager

## Documentation
The documentation for this library can be found at the following URL:

http://ustepper.com/docs/usteppers/html/index.html

### MAC (and a few Windows) Users:
In order to make the uStepper S show up in the "ports" list, you need to install the VCP driver from the following link: 
https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers

## Known bugs
- does not properly release motor in dropin mode

## Change Log
2.3.1:
- added 6 new examples

2.3.0:
- Changed Servo library (uStepperServo.cpp) to use timer PWM instead of polling to control servo motors. 
- Changed the servo library example to use the new servo implementation.
- Changed the 'stop' method at the uStepperS.cpp to wait until the motor realy stops movement. Thanks to Daniel-Walther for pointing it out.

2.2.1:
- Added timeout to homing function

2.2.0:
- Added function to get current driver speed in RPM (getDriverRPM())
- Fixed "getVelocity()" method of "uStepperDriver" class to use correct twos complement conversion
- Updated keywords
- Added two new examples:
	- GYROBalance: Example on how to use MPU6050 IMU with uStepper S 
	- SWiFiGUI: Example sketch to control the uStepper S with browser based GUI over WiFi

2.1.0:
- Fixed checkOrientation to work with Closed loop and dropin modes
- Fixed stallguard feature
- Modified examples to library update  

2.0.0:
- Changed name of "brake()" function in uStepper Class to "setBrakeMode()"
- Implemented "setBrakeMode()" function in uStepper Class to choose between freewheel, braking with low side fets shorted and brake with specified hold current. default = brake with low side fets shorted
- "setHome" argument of ustepper class "setup" function is now used
- Implemented "checkOrientation()" function in uStepper Class, to check the orientation of the motor cable, and invert direction if needed.
- Added 3 moves in "checkOrientation()" function in uStepper Class, to check orientation instead of just 1
- Removed check for motor cable orientation from "setup()" function in uStepper Class. Users actively needs to call the "checkOrientation" function from uStepper Class, if they need this feature, AFTER calling the "setup()" function
- Disabled driver on MCU reset while setting up, to avoid the motor spinning on startup if power was removed during motor movement
- Moved the absolute position counter into the encoder getangle function
- Added a LP filter on the absolute position data in the encoder function (so, getAngleMoved is filtered)
- Stall detection removed from timer1
- Variable filter depending on mode set in setup routine
- Removed obsolete things in setup routine and timer1
- Made new velocity measurement for encoder values
- Made the control threshold for closed loop control variable and intrduced a function for editing it
- Changed timer1 interrupt frequency to 2KhZ for all other modes than DROPIN
- Added option to choose how long the "checkOrientation()" function in UstepperS class should move during check
- Renamed "PID" to "CLOSEDLOOP" to avoid confusing the closed loop position mode with a PID controller. PID keyword is stall accepted for backwards compatibility

1.0.1:
- Fixed bug in functions to set acceleration and deceleration
- moved a couple of functions in uStepperDriver.h from public to protected section of class
- added documentation

1.0.0:
- Bug fixes
- New Dropin PID code
- Added dropin CLI interface
- Fixed stall detection, and added user sensitivity parameter

0.1.1:
- Bug fixes

0.1.0:	
- Initial release

<a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" /></a><br /><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">uStepper</span> by <a xmlns:cc="http://creativecommons.org/ns#" href="www.ustepper.com" property="cc:attributionName" rel="cc:attributionURL">ON Development</a> is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License</a>.
