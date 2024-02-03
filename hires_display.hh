#ifndef HIRES_DISPLAY_H
#define HIRES_DISPLAY_H

#ifdef USE_HIRES_DISPLAY

#include "Adafruit_GFX.h"
#include <GU_K61NA4.h>
#include "Serial_GFX.h"
#include "display.hh"

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

const Fonts font_mono_val_s = {&B612Mono_Regular18pt7b, &B612Mono_Regular18ptDot, &B612Mono_Regular18ptDash, &B612Mono_Regular18ptDeg};
const Fonts font_mono_val_l = {&B612Mono_Regular24pt7b, &B612Mono_Regular24ptDot, &B612Mono_Regular24ptDash, &B612Mono_Regular24ptDeg};
const Fonts font_var_title = {&B612_Bold16pt7b, nullptr, nullptr, nullptr};
const Fonts font_var_lbl = {&B612_Regular10pt7b, nullptr, nullptr, nullptr};
const Fonts font_var_lbl_b = {&B612_Bold7pt7b, nullptr, nullptr, nullptr};

class Hires_Display : public Display
{

protected:

   void drawLabel(uint16_t x, uint16_t y, String label);
   void drawLabel(uint16_t x, uint16_t y, const char *label);

   void drawAltitude();
   void drawVS();
   void drawVSLabel();
   void drawIAS();
   void drawIASLabel();
   void drawHeading();
   void drawHeadingLabel();
   void drawTransponderCode();
   void drawRadioActive();
   void drawRadioStandby();
   void drawBarometer();
   void drawRadioSelectButton(uint8_t active = 0);


   void trimDecimal(float_t num, uint8_t padding, uint8_t decimals, bool dashes, bool force_sign, int x, int y, GFXcanvas1 *canvas, const Fonts *font);
   void printDash(uint8_t num, GFXcanvas1 *canvas, const Fonts *font);
   String getStringValue(String data, char separator, int index);

   GFXcanvas1 cCenter;
   //GU126X32_K612A4 lcd;
   Virtual_HX8757 disp_device;

public:
   Hires_Display();

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
   void updateVS();
   void updateVSLabel();
   void updateIAS();
   void updateIASLabel();
   void updateHeading();
   void updateHeadingLabel();
   void updateTransponderCode();
   void updateRadioFrequencyActive();
   void updateRadioFrequencyStandby();
   void updateActiveRadio();
   void updateBarometer();
   void updateBarometerLabel(uint8_t selection);

   void printSerial();
};

//extern Hires_Display disp;

#endif
#endif