#ifndef LORES_DISPLAY_H
#define LORES_DISPLAY_H

#include "Adafruit_GFX.h"
#include <GU_K61NA4.h>
#include "Serial_GFX.h"

#include <Encoder.h>
#include <Bounce.h>

//#include "display.hh"

#include "TouchScreen.h"
#include "Adafruit_STMPE610.h"


class VBarDef {
public:
    uint8_t x, y1, y2;
    int8_t h;
    uint8_t color = 1;

    VBarDef(uint8_t _x, uint8_t _y1, uint8_t _y2);

    //void draw(GU_K61NA4 _disp);
};

class DisplayTextBox {
public:
    uint8_t x, y, w, h;
    char text[10] = {0};
    bool left_align = true;
    bool active = false;
    bool updated = false;
    const GFXfont *font = &Noritake5x7;
    const char* format = "%d";
    DisplayTextBox(uint8_t _x, uint8_t _y, uint8_t _w, uint8_t _h);
    DisplayTextBox(uint8_t _x, uint8_t _y, uint8_t _w, uint8_t _h,  bool _left_align);
    DisplayTextBox(uint8_t _x, uint8_t _y, uint8_t _w, uint8_t _h,  const GFXfont * _font);
    DisplayTextBox(uint8_t _x, uint8_t _y, uint8_t _w, uint8_t _h,  const GFXfont * _font, bool _left_align);
    DisplayTextBox(uint8_t _x, uint8_t _y, uint8_t _w, uint8_t _h,  const GFXfont * _font, bool _left_align, const char* _format);
    void updateALT(uint16_t _value, int16_t vpower);
    void update(String _text, int16_t vpower);
    void update(float _value, int16_t vpower);
    /*
    void draw(GU_K61NA4 _disp);


    void update(String _text);

    void draw(GU_K61NA4 _disp, String _text);
    */
};


class Lores_Display
{

protected:
   struct Update
   {
      bool alt = false;
      bool vs = false;
      bool vs_lbl = false;
      bool ias = false;
      bool ias_lbl = false;
      bool hdg = false;
      bool hdg_lbl = false;
      bool radio_active = false;
      bool radio_standby = false;
      bool radio_select_buttons = false;
      bool xpdr = false;
      bool baro = false;
   };

   VBarDef **vbars;
   DisplayTextBox **textboxes;
   Encoder **encoders;

   struct Update update;

   void drawLabel(uint16_t x, uint16_t y, String label);
   void drawLabel(uint16_t x, uint16_t y, const char *label);

   Adafruit_STMPE610 ts;

public:
   Lores_Display();

   void initDisplay();
   void printStatic();
   void printSplash();
   void printSplash(String str);
   void printLastCommand(uint8_t command, uint8_t index, int32_t value);
   void printLastCommand(uint8_t command, uint8_t index, const String value);
   void printLastCommand(uint8_t command, uint8_t index, char* value);
   void printDebug(String msg);
   void printMem();
   uint32_t checkEncoders();
   

   void redraw(bool full = false);
   GU126X32_K612A4 disp_device;

   /*
   TouchEvent processTouch();
   void clearTouch();
   
   */

   DisplayTextBox
       LatMode_TextBox,
       LatModeValue_TextBox,
       LatModeArm_TextBox,
       VertMode_TextBox,
       VertModeValue_TextBox,
       VertModeUnits_TextBox,
       VertModeArmLeft_TextBox,
       VertModeArmLeftSpecialMode_TextBox,
       VertModeArmRight_TextBox;

    const uint8_t nBoxes = 9;

   VBarDef
       LeftMarker,
       RightMarker;

    const uint8_t nBars = 2;
    
    Encoder Encoder1;

    const uint8_t nEncoders = 1;

    Bounce button1;

    const uint8_t nButtons = 1;

    int lastFullRedraw = 0;


   void printSerial();
};

extern Lores_Display disp;


#endif