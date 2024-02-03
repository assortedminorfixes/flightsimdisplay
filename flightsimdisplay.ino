#include <Arduino.h>
#include <usb_desc.h>

#include <GU_K61NA4.h>
#include "Serial_GFX.h"

// #include <Fonts/FreeSansOblique18pt7b.h>

/******
 * Connect display to Arduino using these pins:
 * VFD pin  | Name  | Arduino I/O
 * ------------------------------
 *  2       | SIN   | 3
 *  4       | SBUSY | 4
 *  5       | SCK   | 5
 *  6       | RESET | 6
 *
 */

// GU126X32_K612A4 vfd(4, 6, 10);

// Serial_GFX vfd(320, 480);

#include "flightsimdisplay/featherwing_touch.hh"
#include "flightsimdisplay/state.hh"
#include "flightsimdisplay/lores_display.hh"
#include "flightsimdisplay/messaging.hh"
// #include "flightsimdisplay/lights.hh"

Lores_Display disp{};
// LightController lights{};
CommsController comms{};
struct State state;

uint32_t encoders_changed = 0;

u_int FULLREDRAWTIME = 1000 * 60; // Full redraw every minute to handly any graphical glitches

void setup()
{

  // 115200 is typically the maximum speed for serial over USB
  Serial.begin(115200);

  // Initialize the Display including Touch
  disp.initDisplay();

  state.debug = true;
  disp.printSerial();

  disp.disp_device.liveMode = true;

  // Attach my application's user-defined callback methods
  comms.attachCommandCallbacks();

  // Initialize Lights
  // lights.initLights();
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
    /*
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
          disp.updateHeadingLabel();
          break;
        case TouchEventType::BARO_BUTTON:
          comms.updateBaroMode(te.value);
          disp.updateBarometerLabel(te.value);
          break;
        default:
          break;
        }
        state.last_touch = millis();
      }
    }
    else
    {
      disp.clearTouch();
    }
    */

    // Update inputs from encoders
    /*  Consider elegant solution for more encoders later.
    encoders_changed = disp.checkEncoders();
    for(uint8_t i=0; i<disp.nEncoders && encoders_changed != 0; i++){
      if(encoders_changed & 1)
        comms.updateEncoder(disp.readAndResetEncoder(i), i);
      encoders_changed >>=1;
    }
    */

    if (disp.checkEncoders() != 0)
    {
      comms.updateEncoder1(disp.Encoder1.readAndReset() / 4);
    }

    if (disp.button1.update())
    {
      comms.updateButton1(disp.button1.fallingEdge());
    }

    if (millis() - disp.lastFullRedraw > FULLREDRAWTIME)
    {
      disp.printStatic();
      disp.redraw(true);
    }
    else
      disp.redraw();

    // lights.update();

    if (state.debug)
      disp.printMem();
  }

  // If virtual power is on, but the screen layout is not printed,
  // then print the static layout and update course label and redraw
  // data values.
  else if (state.configured && state.power && !state.display_static && state.isReady())
  {
    disp.printStatic();
    disp.redraw(true);
    disp.Encoder1.write(0);
  }

  // If virtual power is off, but the off splash is not printed,
  // then print the off splash.
  else if (state.configured && !state.power && !state.display_off && state.isReady())
  {
    disp.printSplash(F("POWER OFF"));
    // lights.update();
    state.display_off = true;
  }

  disp.printSerial();

} // End of the Main Loop