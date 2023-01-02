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

#define DEBUG

Display disp{};
LightController lights{};

struct State state;

void setup()
{

  // 115200 is typically the maximum speed for serial over USB
  Serial.begin(115200);

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

  if (!state.display_static && state.power)
  {
    disp.printStatic();
    disp.updateCourseLabel(state.nav.crs_sel);
    disp.redraw(true);
  }

  if (state.configured && state.display_static)
  {

    if ((millis() - state.last_touch) > TS_DOUBLETOUCH_DELAY)
    {
      TouchEvent te = disp.processTouch();
      if (te.event != TouchEventType::NO_TOUCH)
      {

        if (te.event == TouchEventType::NAV_BUTTON)
        {
          updateRadioSource(te.value);
          state.last_touch = millis();
        }
        else if (te.event == TouchEventType::CRS_BUTTON)
        {
          updateCourseSource(te.value);
          disp.updateCourseLabel(te.value);
          state.last_touch = millis();
        }
      }
    }
    else
    {
      disp.clearTouch();
    }

    disp.redraw();
    lights.update();

#ifdef DEBUG
    disp.printMem();
#endif
  }

} // End of the Main Loop
