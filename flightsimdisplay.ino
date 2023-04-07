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
CommsController comms{};
struct State state;

void setup()
{

  // 115200 is typically the maximum speed for serial over USB
  Serial.begin(115200);

  // Initialize the Display including Touch
  disp.initDisplay();

  // Attach my application's user-defined callback methods
  comms.attachCommandCallbacks();

  // Initialize Lights
  lights.initLights();
}

// Start up the Main Loop Function
void loop()
{
  // Process incoming serial data, and perform callbacks
  comms.processInputData();

  // If device is configured, powered and initalized, then
  // process touches and perform updates to display and lights.
  if (state.configured && state.power && state.display_static && state.isReady())
  {
    if ((millis() - state.last_touch) > TS_DOUBLETOUCH_DELAY)
    {
      TouchEvent te = disp.processTouch();
      if (te.event != TouchEventType::NO_TOUCH)
      {
        switch (te.event)
        {
        case TouchEventType::NAV_BUTTON:
          comms.updateRadioSource(te.value);
          break;
        case TouchEventType::CRS_BUTTON:
          comms.updateCourseSource(te.value);
          disp.updateCourseLabel(te.value);
          break;
        case TouchEventType::SPEED_BUTTON:
          comms.updateSpeedMode(te.value);
          disp.updateSpeedLabel();
          break;
        case TouchEventType::BARO_BUTTON:
          comms.updateBaroMode(te.value);
          disp.updateBarometerLabel(te.value);
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

    if (state.debug)
      disp.printMem();
  }

  // If virtual power is on, but the screen layout is not printed,
  // then print the static layout and update course label and redraw
  // data values.
  else if (state.configured && state.power && !state.display_static && state.isReady())
  {
    disp.printStatic();
    disp.updateCourseLabel(state.nav.crs_sel);
    disp.updateSpeedLabel();
    disp.updateBarometerLabel(state.nav.baro_mode_sel);
    disp.updateHeadingLabel();
    disp.redraw(true);
  }

  // If virtual power is off, but the off splash is not printed,
  // then print the off splash.
  else if (state.configured && !state.power && !state.display_off && state.isReady()) {
      disp.printSplash(F("-"));
      lights.update();
      state.display_off = true;
  }

} // End of the Main Loop
