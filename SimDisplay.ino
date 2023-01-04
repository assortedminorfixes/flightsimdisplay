/*
 Inspired by Spad.Next Serial Interface v1 Simple Autopilot by Les O'Reilly .
 Using Adafruit Feather with 3.5" Touch Wing.
 Display is 'passive' with exception of touch to change radio and
 Button inputs are separate using a Leo Bodnar BBI-32.

 Current version is extensively using the SUBCRIBE patten in Spad.Next Serial.
 Data Values used can be found in messaging.cpp; it is using Simconnect values
 to improve compatibility.

 Display is based on the B612 font (https://github.com/polarsys/b612), converted for
 use with Adafruit GFX using fontconvert.
*/
#include "state.hh"
#include "display.hh"
#include "messaging.hh"
#include "lights.hh"
#include "featherwing_touch.hh"

Display disp{};
LightController lights{};

struct State state;

void setup()
{

  // 115200 is typically the maximum speed for serial over USB
  Serial.begin(115200);

  // Initialize the Display including Touch
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

  // If virtual power is on, but the screen layout is not printed,
  // then print the static layout and update course label and redraw
  // data values.
  if (state.configured && state.power && !state.display_static)
  {
    disp.printStatic();
    disp.updateCourseLabel(state.nav.crs_sel);
    disp.updateSpeedLabel(state.nav.speed_mode_sel);
    disp.redraw(true);
  }

  // If device is configured, powered and initalized, then
  // process touches and perform updates to display and lights.
  if (state.configured && state.power && state.display_static)
  {
    if ((millis() - state.last_touch) > TS_DOUBLETOUCH_DELAY)
    {
      TouchEvent te = disp.processTouch();
      if (te.event != TouchEventType::NO_TOUCH)
      {
        switch(te.event)
        {
          case TouchEventType::NAV_BUTTON:
            updateRadioSource(te.value);
            break;
          case TouchEventType::CRS_BUTTON:
            updateCourseSource(te.value);
            disp.updateCourseLabel(te.value);
            break;
          case TouchEventType::SPEED_BUTTON:
            updateSpeedMode(te.value);
            disp.updateSpeedLabel(te.value);
            break;
        }
        state.last_touch = millis();             
      }
    }
    else
    {
      disp.clearTouch();
    }

    disp.redraw();
    lights.update();

    if (state.debug) disp.printMem();

  }

} // End of the Main Loop
