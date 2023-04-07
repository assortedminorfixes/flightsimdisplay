#ifndef DISPLAY_H
#define DISPLAY_H

#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"

#include "TouchScreen.h"
#include "Adafruit_STMPE610.h"

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

   GFXcanvas1 cCenter;
   const char SYM_DEG[1] = {0xB0};
   const char SYM_DOT[1] = {0x2E};
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
   void drawLabel(uint16_t x, uint16_t y, const char* label);

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
   void trimDecimal(float_t num, uint8_t padding, uint8_t decimals, bool dashes, bool force_sign, int x, int y, GFXcanvas1 *canvas, const GFXfont *font);
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