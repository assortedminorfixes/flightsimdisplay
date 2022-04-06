/*
 Spad.Next Serial Interface v1 Simple Autopilot by Les O'Reilly .
 Using I2C 2004 LCD, 6 Position Rotary Switch, Rotary Encoder with Button, 8 AP Function buttons

 //Encoder code by Ben Buxton... This is my Go To for encoders...always perfect!!
 //More info: http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html

// SPAD.neXt >= 0.9.10.13 ... Should work with earlier however when I did all this modification this is the version I was On
// SPAD Serial Protocol Version 1  (as of May 8, 2021 there is only version 1 but the future could have changes...)
// Documentation on the Spad Serial Protocol is found here ----  https://github.com/c0nnex/SPAD.neXt/wiki/Serial-Connection
// For now we will need to use vJoy for buttons and Script Panel for Display Data
*/
#include "display.hh"
#include "messaging.hh"
#include "lights.hh"
#include "featherwing_touch.hh"

Display disp{};
LightController lights{};

struct State
{
   uint8_t radio = 0;
   uint8_t crs = 0;
   long last_touch = 0;
} state;

void setup()
{

  // 115200 is typically the maximum speed for serial over USB
  Serial.begin(9600);

  disp.initDisplay();

  // Attach my application's user-defined callback methods
  attachCommandCallbacks();

  // Initialize Lights
  lights.initLights();

}

// Start up the Main Loop Function
void loop()
{
  // Process incoming serial data, and perform callbacks
  messenger.feedinSerialData();
 
  TouchEvent te = disp.processTouch();
  if ((millis() - state.last_touch) > 500 && (te.event != TouchEventType::NO_TOUCH)) {
    
    if (te.event == TouchEventType::NAV_BUTTON && te.value != state.radio) {
          updateRadioSource(te.value);
          state.radio = te.value;
          state.last_touch = millis();
    } else if (te.event == TouchEventType::CRS_BUTTON && te.value != state.crs) {
          updateCourseSource(te.value);
          disp.updateCourseLabel(te.value);
          state.crs = te.value;
          state.last_touch = millis();
    }
  }

  disp.redraw();
  lights.update();

} // End of the Main Loop
