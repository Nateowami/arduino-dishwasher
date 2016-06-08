## Arduino Dishwasher

This repository is for an Arduino sketch for running a dishwasher. It's intended for the dishwasher build over at http://www.instructables.com/id/Building-a-Dishwasher-From-Scratch, but it could work with similar builds as well.

### Usage

Download the main file (arduino-dishwasher.ino) and open it with the [Arduino IDE](https://www.arduino.cc/en/Main/Software). The sketch has a few dependencies. Go to Sketch > Include Library > Manage Libraries..., and install `RBD_Timer`, `RBD_Button`, and `elapsedMillis`. For more detailed instructions on installing libraries, visit https://www.arduino.cc/en/Guide/Libraries.

Change any pinout as needed (all the pins are specified near the top of the file) and upload the code to an Arduino. I've only used it on the Arduino Uno, but in theory it ought to work fine on any board, as long as the pin numbers are mapped to available pins.

### Features

Currently the interface works with a 16x2 LCD and three input buttons. The middle button is used to start a cycle, at which point the software starts a countdown until the cycle is done. The user can use the right button to pause the cycle (this is indicated on the display), and can then choose to resume or stop the cycle. Currently a cycle simply consists of turning on two solenoid valves, and lasts 90 minutes (easily configurable). The drain pump will run when water is detected, and a piezo will beep when the cycle is done. It automatically determines (using thermistors) which solenoid controls hot water, and which controls cold (just a convenience feature which could easily be removed).so

### TODO

* Add support for selecting cycles
* Make cycles more advanced and easily configurable (rinse cold, then warm, etc.)
* Test rigorously
