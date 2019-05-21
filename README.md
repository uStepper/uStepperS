# uStepper S

The library contains support for driving the stepper, reading out encoder data. A few examples are included to show the functionality of the library.
The library is supported and tested with in Arduino IDE 1.8.8.

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
