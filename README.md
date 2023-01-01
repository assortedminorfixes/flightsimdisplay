# Flight Simulator Display

Using Arduino to create the display part of a pseudo Flight Control Unit

Inspired by Spad.Next Serial Interface v1 Simple Autopilot by Les O'Reilly. Using Adafruit Feather with 3.5" Touch Wing. Display is 'passive' with exception of touch to change radio and course.Button inputs are separate using a Leo Bodnar BBI-32.

Current version is extensively using the SUBCRIBE patten in Spad.Next Serial. Data Values used can be found in messaging.cpp; it is using Simconnect values to improve compatibility.

Display is based on the B612 font (https://github.com/polarsys/b612), converted for use with Adafruit GFX using fontconvert.

## Hardware

The recommended hardware is the following:
* Adafruit 3.5" Touch Wing (https://learn.adafruit.com/adafruit-3-5-tft-featherwing)
* Adafruit Feather M4 Express (https://learn.adafruit.com/adafruit-feather-m4-express-atsamd51)
* 6 NeoPixels in one string, connected to pin 14

It is possible to use the Feather M0 Express, but given the rather compute heavy display updates the M4 is recommended. 

## Arduino Libraries

The following libraries are required:
* CmdMessenger (https://github.com/thijse/Arduino-CmdMessenger)
* Adafruit NeoPixel
* Adafruit GFX
* Adafruit HX8357
* Adafruit STMPE610

## SPAD.Next

The interface to Mcrosoft FlightSimulator goes via [SPAD.Next](https://www.spadnext.com/home.html); the device must be set up as a Serial device.