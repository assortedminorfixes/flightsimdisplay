#include <Arduino.h>

#include "lores_display.hh"
#include "messaging.hh"
#include "featherwing_touch.hh"
#include "state.hh"
#include "Serial_GFX.h"

#include "fonts/u8g_font_7x14.h"

#include <SPI.h>

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char *sbrk(int incr);
#else  // __Arm__
extern char *__brkval;
#endif // __arm__

/*
int freeMemory()
{
    char top;
#ifdef __arm__
    return &top - reinterpret_cast<char *>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
    return &top - __brkval;
#else  // __arm__
    return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif // __arm__
}

template <typename T>
int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}
*/


#define DECIMAL_PAD 5
#define DEBUG_STR_LENGTH 25
/*

// Original approach, all in program on Arduino.  Would require recompile if the logic ever changes.
struct LabelMarker {
    uint8_t x;
    uint8_t y;
    const char* text;
    bool left_align = true;
    bool active = false;
    const GFXfont *font = &Noritake5x7;

    LabelMarker(const char* _text) {
        text = _text;
    };
    LabelMarker() {
    };

    void draw(GU_K61NA4 display) {
        display.setFont(font);
        display.setCursor(x, y);
        if (left_align){
            display.print(text);
        }
        else
            display.printf_r(text);

    };
};

struct SmallLabelMarker : LabelMarker {
    const GFXfont *font = &Noritake5w;
    SmallLabelMarker(const char* _text) : LabelMarker(_text)
    {
    };
};

struct LM_LabelMarker : LabelMarker {
    LM_LabelMarker(const char* _text) : LabelMarker(_text) {
        x = 2;
        y = 8;
    };
};

struct LM_Arm_LabelMarker : SmallLabelMarker {
    LM_Arm_LabelMarker(const char* _text) : SmallLabelMarker(_text) {
        x = 2;
        y = 28;
    };
};

struct VM_LabelMarker : LabelMarker {
    VM_LabelMarker(const char* _text) : LabelMarker(_text) {
        x = 26;
        y = 8;
    };
};

struct VM_Value_Units_Marker : SmallLabelMarker {
    VM_Value_Units_Marker(const char *_text) : SmallLabelMarker(_text ){
        x = 76;
        y = 14;
        left_align = false;
    };
};

struct VM_Value_Marker : LabelMarker {
    char text[10] = {0};
    VM_Value_Marker(){
        x = 76;
        y = 8;
        left_align = false;
    };
};

struct VM_LabelValue_Marker : VM_LabelMarker{
    VM_Value_Units_Marker *units = NULL;
    VM_Value_Marker value_marker;
    DisplayField *value = NULL;
    const char* format = "%d";
    VM_LabelValue_Marker(DisplayField *_field, VM_Value_Units_Marker *_units) : VM_LabelMarker(const_cast<char *>(_field->label.c_str())) {
        x = 76;
        y = 8;
        left_align = false;
        units = _units;
    };

    VM_LabelValue_Marker(DisplayField *_field, VM_Value_Units_Marker *_units, const char* _format)
             : VM_LabelMarker(const_cast<char *>(_field->label.c_str())){
        VM_LabelValue_Marker(_field, _units);
        format = _format;
    };

    void draw(GU_K61NA4 _disp){
        sprintf(value_marker.text, format, value->value);
        LabelMarker::draw(_disp);
        value_marker.draw(_disp);
        units->draw(_disp);
    };
};

struct VM_Arm_LabelMarker : LabelMarker {
    VM_Arm_LabelMarker(const char* _text) : LabelMarker(_text) {
        x = 26;
        y = 28;
    };
};

struct VM_Arm_Special_LabelMarker : LabelMarker {
    VM_Arm_Special_LabelMarker(const char* _text) : LabelMarker(_text) {
        x = 26;
        y = 21;
    };
};

struct VM_Arm2_LabelMarker : LabelMarker {
    VM_Arm2_LabelMarker(const char* _text) : LabelMarker(_text) {
        x = 76;
        y = 28;
        left_align = false;
    };
};

struct VBarDef {
    uint8_t x;
    uint8_t y1;
    uint8_t y2;
    uint8_t color = 1;

    void draw(GU_K61NA4 _disp) {
        _disp.drawFastVLine(x, y1, y2-y1+1, color);
    }
};

struct VBarDef LeftMarker = {24, 3, 27};
struct VBarDef RightMarker = {78, 3, 27};

struct LM_LabelMarker 
    LM_ROL_LABEL("ROL"),
    LM_GPS_LABEL("GPS"),
    LM_HDG_LABEL("HDG"),
    LM_BC_LABEL("BC"),
    LM_VOR_LABEL("VOR"),
    LM_LOC_LABEL("LOC"),
    LM_LVL_LABEL("LVL"),
    LM_GA_LABEL("GA"),
    LM_VAPP_LABEL("VAPP");

struct LM_Arm_LabelMarker 
    LM_Arm_ROL_LABEL("ROL"),
    LM_Arm_GPS_LABEL("GPS"),
    LM_Arm_HDG_LABEL("HDG"),
    LM_Arm_BC_LABEL("BC"),
    LM_Arm_VOR_LABEL("VOR"),
    LM_Arm_LOC_LABEL("LOC"),
    LM_Arm_LVL_LABEL("LVL"),
    LM_Arm_GA_LABEL("GA"),
    LM_Arm_VAPP_LABEL("VAPP");

struct VM_LabelMarker
    VM_PIT_LABEL("PIT"),
    VM_GS_LABEL("GS"),
    VM_VNAV_LABEL("VNAV"),
    VM_GP_LABEL("GP"),
    VM_GA_LABEL("GA"),
    VM_LVL_LABEL("LVL"),
    VM_VPTH_LABEL("VPTH");

struct VM_Value_Units_Marker 
    VM_FT_UNIT_LABEL("FT"),
    VM_FPM_UNIT_LABEL("FPM"),
    VM_KT_UNIT_LABEL("KT");


struct VM_LabelValue_Marker
    VM_ALT_LABEL_VALUE(&state.nav.alt, &VM_FT_UNIT_LABEL),
    VM_ALTS_LABEL_VALUE(&state.nav.alt, &VM_FT_UNIT_LABEL),
    VM_VS_LABEL_VALUE(&state.nav.vs, &VM_FPM_UNIT_LABEL),
    VM_IAS_LABEL_VALUE(&state.nav.ias, &VM_KT_UNIT_LABEL);


struct VM_Arm_LabelMarker
    VM_Arm_VS_LABEL("VS"),
    VM_Arm_ALT_LABEL("ALT"),
    VM_Arm_PIT_LABEL("PIT"),
    VM_Arm_GS_LABEL("GS"),
    VM_Arm_VNAV_LABEL("VNAV"),
    VM_Arm_GP_LABEL("GP"),
    VM_Arm_IAS_LABEL("IAS"),
    VM_Arm_GA_LABEL("GA"),
    VM_Arm_LVL_LABEL("LVL"),
    VM_Arm_ALTS_LABEL("ALTS");
    
    // Special, see notes
struct VM_Arm_Special_LabelMarker
    VM_Arm_Special_VPTH_LABEL("VPTH"),
    VM_Arm_Special_ALTS_LABEL("ALTS"),
    VM_Arm_Special_ALTV_LABEL("ALTV");

struct VM_Arm2_LabelMarker
    VM_Arm2_ALTS_LABEL("ALTS"),
    VM_Arm2_ALTV_LABEL("ALTV"),
    VM_Arm2_GP_LABEL("GP"),
    VM_Arm2_GS_LABEL("GS"),
    VM_Arm2_VPTH_LABEL("VPTH");

*/

// New Approach, define the regions based on Display and provide those as DISPLAY options for SPAD.neXt

/*
Virtual_HX8757::Virtual_HX8757(uint8_t pin1, uint8_t pin2, uint8_t pin3) : Serial_GFX(320, 480)
{
    raw = true;
}
*/


/* Loresolution display is based on the Garmin GFC 600 series autopilot display.

Cast onto a 126x32 VFD

Vertical bars dividing horizontal and vertical AP control modes and messages.

First bar: x=24, y=3..27
Second bar: x=78, y=3..27

Active mode uses ~5x7 text, queued mode uses ~4x5 text, can use the inbuilt Noritake5x7 and 5w fonts.

Active lateral mode cursor: 2,8
    inact l mode cursor: 2, 28
Active vertical mode cursor: 27, 8
    inact v mode cursor: 26, 28


Possible LM labels: ROL, GPS, HDG, BC, VOR, LOC, LVL, GA

    Special modes:  VAPP := Approach VOR capture/track


Possible VM labels: VS, ALT, PIT, GS, VNAV, GP, IAS, GA, LVL, ALTS, VPTH
    Special modes:  ALTS := Altitude capture
                    ALTV := Vertical path constraint altitude capture
                    VPTH := vertical path active
                    GP   := Glide path (SBAS on GPS approach)
                    GS   := Glide slope ILS

VM Data:
        ALT := nnnnn FT (small FT)
                    When adjusting up/down it will replace the altitude readout with +/- XXXX FT
                Position of Altitude readout is right aligned to cursor (76, 8) with FT below
                    right aligned to (76, 14)

        ALTS:= nnnnn FT
                Will appear when gps is approaching ALT set, prior to that it appears in the armed modes
        VS  := nnnn FPM (small FPM)
                Has an up or down arrow (could be stored in a macro on the VFD).  VS is left aligned,
                nnnn is right aligned, FPM is below in small text like ALT, same coordinates.
                (Arrow shape appears to be from top to bottom in centered pixels: 1, 2, 4, 1, 1, 1, 1 )

        IAS := nnn KT (")
                Same positioning as above.

        VPTH:
                When set, will mark ALTV in the armed mode with ALTV @ (26, 20) and the alt setting @(26, 27)
                Also will put GP or GS on the right aligned @(76, 27) position when active with VPTH

VM notes GP only shown when GPS is LM

Possible annunciators:
    All are right aligned at edge with small font @(122, 7 for start)

    PFT     // Pre flight test
    PFT/AP/YD FAIL
    P/Y/R TRIM FAIL
    ELE/AIL/RUD TRM UP/DN
    MAX/MINSPEED
    AIRDAT FAIL
    ESP OFF
    ESP FAIL
    RB ACTIVE
    RB OFF
    RB FAIL
    CWS ON
    SET HDG=CRS
    DISABLD KEY
    AP/FD/YD/HDG/NAV/APR/BC/VNV/IAS/VS/ALT/LVL KEY STK


There is no option to enable LM indicator, but there doesn't appear to be anything preventing
    using the space below the marker for HDG mode to set the current heading bug value up.    Cast onto a 126x32 VFD
    
    Vertical bars dividing horizontal and vertical AP control modes and messages.
    
    First bar: x=24, y=3..27
    Second bar: x=78, y=3..27
    
    Active mode uses ~5x7 text, queued mode uses ~4x5 text, can use the inbuilt Noritake5x7 and 5w fonts.
    
    Active lateral mode cursor: 2,8
        inact l mode cursor: 2, 28
    Active vertical mode cursor: 27, 8
        inact v mode cursor: 26, 28
        inact v mode SPECIAL cursor: 26, 21
    
    
    Possible LM labels: ROL, GPS, HDG, BC, VOR, LOC, LVL, GA
    
        Special modes:  VAPP := Approach VOR capture/track
    
    
    Possible VM labels: VS, ALT, PIT, GS, VNAV, GP, IAS, GA, LVL, ALTS, VPTH
        Special modes:  ALTS := Altitude capture
                        ALTV := Vertical path constraint altitude capture
                        VPTH := vertical path active
                        GP   := Glide path (SBAS on GPS approach)
                        GS   := Glide slope ILS

        NOTE: inactive vertical mode 
  
  		    
    
    VM Data:
            ALT := nnnnn FT (small FT)
                        When adjusting up/down it will replace the altitude readout with +/- XXXX FT
                    Position of Altitude readout is right aligned to cursor (76, 8) with FT below
                        right aligned to (76, 14)
    
            ALTS:= nnnnn FT
                    Will appear when gps is approaching ALT set, prior to that it appears in the armed modes
            VS  := nnnn FPM (small FPM)
                    Has an up or down arrow (could be stored in a macro on the VFD).  VS is left aligned,
                    nnnn is right aligned, FPM is below in small text like ALT, same coordinates.
                    (Arrow shape appears to be from top to bottom in centered pixels: 1, 2, 4, 1, 1, 1, 1 )
    
            IAS := nnn KT (")
                    Same positioning as above.
    
            VPTH:
                    When set, will mark ALTV in the armed mode with ALTV @ (26, 20) and the alt setting @(26, 27)
                    Also will put GP or GS on the right aligned @(76, 27) position when active with VPTH
    
    VM notes GP only shown when GPS is LM
    
    Possible annunciators:
        All are right aligned at edge with small font @(122, 7 for start)
    
        PFT     // Pre flight test
        PFT/AP/YD FAIL
        P/Y/R TRIM FAIL
        ELE/AIL/RUD TRM UP/DN
        MAX/MINSPEED
        AIRDAT FAIL
        ESP OFF
        ESP FAIL
        RB ACTIVE
        RB OFF
        RB FAIL
        CWS ON
        SET HDG=CRS
        DISABLD KEY
        AP/FD/YD/HDG/NAV/APR/BC/VNV/IAS/VS/ALT/LVL KEY STK
    
    
    There is no option to enable LM indicator, but there doesn't appear to be anything preventing
        using the space below the marker for HDG mode to set the current heading bug value up.


Splash screen:
             GFC 600                //10px font @(42, 11)
    WITH ELECTRONIC STABILITY       // 7px font @(19, 19)
        AND PROTECTION              // 7px font @(34, 28)
    <- CONT.            V 2.98108   // 7px font L@(6, 31) R@(95, 31) <- is a special character




*/
VBarDef::VBarDef(uint8_t _x, uint8_t _y1, uint8_t _y2){
    x = _x;
    y1 = _y1;
    y2 = _y2;
    h = _y2 - _y1;
};

DisplayTextBox::DisplayTextBox(uint8_t _x, uint8_t _y, uint8_t _w, uint8_t _h){
    x = _x;
    y = _y;
    w = _w;
    h = _h;
};

DisplayTextBox::DisplayTextBox(uint8_t _x, uint8_t _y, uint8_t _w, uint8_t _h,  bool _left_align){
    x = _x;
    y = _y;
    w = _w;
    h = _h;
    left_align = _left_align;
};

DisplayTextBox::DisplayTextBox(uint8_t _x, uint8_t _y, uint8_t _w, uint8_t _h,  const GFXfont * _font){
    x = _x;
    y = _y;
    w = _w;
    h = _h;
    font = _font;
};

DisplayTextBox::DisplayTextBox(uint8_t _x, uint8_t _y, uint8_t _w, uint8_t _h,  const GFXfont * _font, bool _left_align){
    x = _x;
    y = _y;
    w = _w;
    h = _h;
    left_align = _left_align;
    font = _font;
}
DisplayTextBox::DisplayTextBox(uint8_t _x, uint8_t _y, uint8_t _w, uint8_t _h, const GFXfont *_font, bool _left_align, const char *_format){
    x = _x;
    y = _y;
    w = _w;
    h = _h;
    left_align = _left_align;
    font = _font;
    format = _format;
};

void DisplayTextBox::updateALT(uint16_t _value, int16_t vpower)
{
    memset(text, 0, sizeof(text));

    if (_value > 18000)
    {
        uint8_t ivalue = _value / 100;
        snprintf(text, sizeof(text) - 1, "FL%03d", ivalue);
    }
    else
        snprintf(text, sizeof(text) - 1 , "%d", _value);

    if (vpower == 0)
        active = 0;
    else if (vpower == 1)
        active = 1;
    updated = true;
    disp.disp_device.debug_printf("DTB::up(text,power) (%d, %d) (%dx%d) L:%d A:%d U:%d, f:%p, S: %s\n",
                        x, y, w, h, left_align, active, updated, font, text);
};

void DisplayTextBox::update(float _value, int16_t vpower)
{
    memset(text, 0, sizeof(text));
    snprintf(text, sizeof(text) - 1, format, _value);
    if (vpower == 0)
        active = 0;
    else if (vpower == 1)
        active = 1;
    updated = true;
    disp.disp_device.debug_printf("DTB::up(text,power) (%d, %d) (%dx%d) L:%d A:%d U:%d, f:%p, S: %s\n",
                        x, y, w, h, left_align, active, updated, font, text);
};

void DisplayTextBox::update(String _text, int16_t vpower)
{
    memset(text, 0, sizeof(text));
    strncpy(text, const_cast<char *>(_text.c_str()), sizeof(text) - 1);
    if (vpower == 0)
        active = 0;
    else if (vpower == 1)
        active = 1;
    updated = true;
    disp.disp_device.debug_printf("DTB::up(text,power) (%d, %d) (%dx%d) L:%d A:%d U:%d, f:%p, S: %s\n",
                        x, y, w, h, left_align, active, updated, font, text);
};

/*
void VBarDef::draw(GU_K61NA4 _disp) {
    _disp.drawFastVLine(x, y1, y2-y1+1, color);
};

void DisplayTextBox::draw(GU_K61NA4 _disp)
{
    _disp.debug_printf("DTB::draw (%d, %d) (%dx%d) L:%d A:%d U:%d, f:%p, S: %s\n",
                        x, y, w, h, left_align, active, updated, font, text);
    //_disp.fillRect(x, y - h, w, h, 0);
    if (active)
    {
        _disp.setFont(font);
        _disp.fillRect(x, y - h, w, h, 0);
        _disp.setCursor(x, y);
        if(left_align)
            _disp.print(text);
        else
            _disp.printf_r(text);
    }
};

void DisplayTextBox::update(String _text)
{
    disp.disp_device.debug_printf("DTB::up(text) (%d, %d) (%dx%d) L:%d A:%d U:%d, f:%p, S: %s\n",
                        x, y, w, h, left_align, active, updated, font, text);
    memset(text, 0, sizeof(text));
    strncpy(text, const_cast<char *>(_text.c_str()), sizeof(text) - 1);
    updated = true;
};

void DisplayTextBox::draw(GU_K61NA4 _disp, String _text)
{
    update(_text);
    draw(_disp);
};
*/
Lores_Display::Lores_Display() : disp_device(TFT_DC, TFT_RST, TFT_CS, TFT_DIN, TFT_DOUT, TFT_CLK),
                                 LatMode_TextBox(2, 8, 18, 8),
                                 LatModeValue_TextBox(2, 18, 18, 6, &Noritake5w),
                                 LatModeArm_TextBox(2, 28, 18, 6, &Noritake5w),
                                 VertMode_TextBox(24, 8, 20, 8),
                                 VertModeValue_TextBox(80, 8, 30, 8, &Noritake5x7, false),
                                 VertModeUnits_TextBox(80, 14, 20, 6, &Noritake5w, false),
                                 VertModeArmLeft_TextBox(24, 28, 40, 6, &Noritake5w),
                                 VertModeArmLeftSpecialMode_TextBox(24, 21, 20, 6, &Noritake5w),
                                 VertModeArmRight_TextBox(80, 28, 20, 6, &Noritake5w, false),
                                 LeftMarker(21, 3, 27),
                                 RightMarker(82, 3, 27),
                                 Encoder1(ENC1P1, ENC1P2),
                                 button1(ENC1SW1, DEBOUNCETIME)
{
    vbars = new VBarDef*[2] {&LeftMarker, &RightMarker};
    textboxes = new DisplayTextBox *[9]
    { &LatMode_TextBox, &LatModeValue_TextBox,
          &LatModeArm_TextBox, &VertMode_TextBox,
          &VertModeValue_TextBox, &VertModeUnits_TextBox, &VertModeArmLeft_TextBox,
          &VertModeArmLeftSpecialMode_TextBox, &VertModeArmRight_TextBox };

    encoders = new Encoder *[1] { &Encoder1 };

    pinMode(ENC1SW1, INPUT);
    digitalWrite(ENC1SW1, HIGH);

    /*
    vbars[0] = &LeftMarker;
    vbars[1] = &RightMarker;
    textboxes[0] = &LatMode_TextBox;
    textboxes[1] = &LatModeArm_TextBox;
    textboxes[2] = &VertMode_TextBox;
    textboxes[3] = &VertModeValue_TextBox;
    textboxes[4] = &VertModeUnits_TextBox;
    textboxes[5] = &VertModeArmLeft_TextBox;
    textboxes[6] = &VertModeArmLeftSpecialMode_TextBox;
    textboxes[7] = &VertModeArmRight_TextBox;
    */
}

void Lores_Display::initDisplay()
{
    // disp_device SETUP
    disp_device.hardReset();
    disp_device.fillScreen(0); // initialize the disp_device
    disp_device.setRotation(0);

    disp_device.setFontFromNative(fN10x14);

    //disp_device.printCentered(F("Init..."));
    printSplash();
    //disp_device.setCursorFlash(true);
}

void Lores_Display::printSplash()
{
    state.display_static = false;
    disp_device.fillScreen(0);
    disp_device.setFont(&u8g_font_7x14);
    disp_device.printCentered("GFC 600", 0, 11, -1, 0);
    disp_device.setFont(&Noritake5w);
    disp_device.printCentered("WITH ELECTRONIC STABILITY", 0, 19, -1, 0);
    disp_device.println();
    disp_device.printCentered("AND PROTECTION", 0, -1, -1, 0);
    disp_device.setCursor(0, 31);
    disp_device.print("<CONT.");
    disp_device.setCursor(125, 31);
    disp_device.printf_r("V 2.98108");
}


void Lores_Display::printSplash(String str)
{
    state.display_static = false;
    disp_device.fillScreen(0);
    disp_device.setFont(&Noritake10x14);
    disp_device.printCentered(str);
}

void Lores_Display::printStatic()
{
    disp_device.fillScreen(0); // initialize the disp_device

    //disp_device.drawFastVLine(25, 3, 23, 1);

    // Only have the two lines as part of the static powered display.
    for (uint8_t i=0; i < nBars; i++){
        disp_device.drawFastVLine(vbars[i]->x, vbars[i]->y1, vbars[i]->h, 1);
        //disp_device.fillRect(vbars[i]->x, vbars[i]->y1, 1, vbars[i]->h, 1);
    }
    //LeftMarker.draw(disp_device);
    //RightMarker.draw(disp_device);

    state.display_static = true;
    state.display_off = false;
}

void Lores_Display::drawLabel(uint16_t x, uint16_t y, const char *label)
{
    disp_device.setCursor(x, y);
    disp_device.printf("%s", label);
}

void Lores_Display::drawLabel(uint16_t x, uint16_t y, String label)
{
    this->drawLabel(x, y, label.c_str());
}

void Lores_Display::redraw(bool full)
{
    if (full)
        lastFullRedraw = millis();
    DisplayTextBox * tb;
    for(uint8_t i=0; i< nBoxes; i++)
    {
        tb = textboxes[i];
        if(full || tb->updated)
        {
            if (tb->left_align)
            {
                disp_device.debug_printf("filling %dx%d @(%d, %d)\n", tb->w, tb->h, tb->x, tb->y - tb->h);
                disp_device.fillRect(tb->x, tb->y - tb->h,
                                     tb->w, tb->h, 0);
            }
            else{
                disp_device.fillRect(tb->x - tb->w - 1, tb->y - tb->h,
                                     tb->w + 2, tb->h, 0);
            }
            if (tb->active){
                disp_device.debug_printf("%d active: %s\n", i, tb->text);
                disp_device.setFont(tb->font);
                disp_device.setCursor(tb->x, tb->y);
                if(tb->left_align)
                    disp_device.print(tb->text);
                else
                    disp_device.printf_r(tb->text);
                tb->updated = false;
            }
        }
    }
}

void Lores_Display::printLastCommand(uint8_t command, uint8_t idx, int32_t val)
{
    this->printLastCommand(command, idx, String(val));
}

void Lores_Display::printLastCommand(uint8_t command, uint8_t idx, char* val)
{
    disp_device.debug_printf("Cmd: %d %d %s", command, idx, val);
}

void Lores_Display::printLastCommand(uint8_t command, uint8_t idx, const String val)
{
    disp_device.debug_printf("Cmd: %d %d %s", command, idx, val);
}

void Lores_Display::printMem()
{
    //disp_device.debug_printf("Mem: %d", freeMemory());
}

uint32_t Lores_Display::checkEncoders()
{
    uint32_t enc_changed = 0;
    for (uint i=0; i<nEncoders; i++){
        enc_changed += bool((encoders[i]->read() / 4) != 0) << i;
    }
    return enc_changed;
}

void Lores_Display::printDebug(String msg)
{
    disp_device.debug_printf(msg.c_str());
}

void Lores_Display::printSerial()
{
    disp_device.printSerial();
}