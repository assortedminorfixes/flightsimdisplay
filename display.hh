#ifndef DISPLAY_H
#define DISPLAY_H

#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"

#include "TouchScreen.h"
#include "Adafruit_STMPE610.h"

#include "fonts/b612reg10pt.h"
#include "fonts/b612bold7pt.h"
#include "fonts/b612bold16pt.h"
#include "fonts/b612monoreg18pt.h"
#include "fonts/b612monoreg18pt-deg.h"
#include "fonts/b612monoreg18pt-dot.h"
#include "fonts/b612monoreg18pt-dash.h"
#include "fonts/b612monoreg24pt.h"
#include "fonts/b612monoreg24pt-deg.h"
#include "fonts/b612monoreg24pt-dot.h"
#include "fonts/b612monoreg24pt-dash.h"

struct Fonts
{
   const GFXfont *normal;
   const GFXfont *dot;
   const GFXfont *dash;
   const GFXfont *deg;
};

const Fonts font_mono_val_s = {&B612Mono_Regular18pt7b, &B612Mono_Regular18ptDot, &B612Mono_Regular18ptDash, &B612Mono_Regular18ptDeg};
const Fonts font_mono_val_l = {&B612Mono_Regular24pt7b, &B612Mono_Regular24ptDot, &B612Mono_Regular24ptDash, &B612Mono_Regular24ptDeg};
const Fonts font_var_title  = {&B612_Bold16pt7b, nullptr, nullptr, nullptr};
const Fonts font_var_lbl    = {&B612_Regular10pt7b, nullptr, nullptr, nullptr};
const Fonts font_var_lbl_b  = {&B612_Bold7pt7b, nullptr, nullptr, nullptr};

enum TouchEventType
{
   NO_TOUCH,
   NAV_BUTTON,
   CRS_BUTTON,
   SPEED_BUTTON,
   BARO_BUTTON
};

class TouchEvent
{
public:
   TouchEventType event;
   uint8_t value;

   TouchEvent() : event(TouchEventType::NO_TOUCH), value(0) {}
   TouchEvent(TouchEventType e, uint8_t v) : event(e), value(v) {}
};

class Display
{

private:
   const char *RADIO_BUTTON_LABEL[5] = {"NAV1", "NAV2", "COM1", "COM2",
                                        "ADF"};
   const char *CRS_LABEL[3] = {"OBS 1", "OBS 2", "GPS DTK"};
   const char *SPEED_LABEL[2] = {"V/S", "Speed"};
   const char *HEADING_LABEL[1] = {"Heading"};
   const char *BARO_LABEL[2] = {"Baro hPa", "Baro inHg"};

   const char SYM_DEG[1] = {0xB0};
   const char SYM_DOT[1] = {0x2E};

   GFXcanvas1 cCenter;
   String dbg_str;

   const uint8_t crs_labels = sizeof(CRS_LABEL) / sizeof(CRS_LABEL[0]);
   const uint8_t speed_labels = sizeof(SPEED_LABEL) / sizeof(SPEED_LABEL[0]);
   const uint8_t baro_labels = sizeof(BARO_LABEL) / sizeof(BARO_LABEL[0]);

   struct Update
   {
      bool alt = false;
      bool speed = false;
      bool speed_lbl = false;
      bool hdg = false;
      bool hdg_lbl = false;
      bool crs = false;
      bool radio_active = false;
      bool radio_standby = false;
      bool radio_select_buttons = false;
      bool xpdr = false;
      bool baro = false;
   };

   struct Update update;

   void drawLabel(uint16_t x, uint16_t y, String label);
   void drawLabel(uint16_t x, uint16_t y, const char *label);

   void drawAltitude();
   void drawSpeed();
   void drawSpeedLabel();
   void drawHeading();
   void drawHeadingLabel();
   void drawCourse();
   void drawTransponderCode();
   void drawRadioActive();
   void drawRadioStandby();
   void drawBarometer();
   void drawRadioSelectButton(uint8_t active = 0);

   Adafruit_HX8357 lcd;
   Adafruit_STMPE610 ts;
   void trimDecimal(float_t num, uint8_t padding, uint8_t decimals, bool dashes, bool force_sign, int x, int y, GFXcanvas1 *canvas, const Fonts *font);
   void printDash(uint8_t num, GFXcanvas1 *canvas, const Fonts *font);
   String getStringValue(String data, char separator, int index);

public:
   Display();

   void initDisplay();
   void printStatic();
   void printSplash(String str);
   void printLastCommand(uint8_t command, uint8_t index, int32_t value);
   void printLastCommand(uint8_t command, uint8_t index, const String value);
   void printDebug(String msg);
   void printMem();

   void redraw(bool full = false);

   TouchEvent processTouch();
   void clearTouch();

   void updateAltitude();
   void updateSpeed();
   void updateSpeedLabel();
   void updateHeading();
   void updateHeadingLabel();
   void updateCourse();
   void updateCourseLabel(uint8_t selection);
   void updateTransponderCode();
   void updateRadioFrequencyActive();
   void updateRadioFrequencyStandby();
   void updateActiveRadio();
   void updateBarometer();
   void updateBarometerLabel(uint8_t selection);
};

extern Display disp;

#endif