#include "Arduino.h"

#include "display.hh"
#include "featherwing_touch.hh"

#include "fonts/b612reg10pt.h"
#include "fonts/b612bold7pt.h"
#include "fonts/b612bold16pt.h"
#include "fonts/b612monobld18pt.h"
#include "fonts/b612monobld24pt.h"

#include <SPI.h>

#define HX8357_CUST_GRAY 0x9CD3

#define CANVAS_NUM_H 45
#define CANVAS_NUM_W 160
#define CANVAS_NUM_LARGE_H 50
#define CANVAS_NUM_LARGE_W 160
#define CANVAS_CENTER_H 60
#define CANVAS_CENTER_W 200

#define RADIO_BUTTON_H 30
#define RADIO_BUTTON_V 50
#define RADIO_BUTTON_RAD 1
#define RADIO_BUTTON_POS_Y 90
#define RADIO_BUTTON_TXTPOS_Y (RADIO_BUTTON_POS_Y + 20)

#define DATA_RADIO_ACT_POS_X 0
#define DATA_RADIO_ACT_POS_Y 25

#define DATA_RADIO_STANDBY_POS_X 160
#define DATA_RADIO_STANDBY_POS_Y 25

#define DATA_ALT_POS_X 0
#define DATA_ALT_POS_Y 180

#define DATA_VS_POS_X 160
#define DATA_VS_POS_Y 180

#define DATA_HDG_POS_X 0
#define DATA_HDG_POS_Y 265

#define DATA_CRS_POS_X 160
#define DATA_CRS_POS_Y 265

#define DATA_XPDR_POS_X 0
#define DATA_XPDR_POS_Y 360

#define DATA_BARO_POS_X 160
#define DATA_BARO_POS_Y 360

#define CENTER_LABEL_POS_X 60
#define CENTER_LABEL_POS_Y 160

#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

static const char *const RADIO_BUTTON_LABEL[] = {"NAV1", "NAV2", "COM1", "COM2",
                                                 "ADF"};

Display::Display() : cNum(CANVAS_NUM_W, CANVAS_NUM_H),
                     cNumLarge(CANVAS_NUM_LARGE_W, CANVAS_NUM_LARGE_H),
                     cCenter(CANVAS_CENTER_W, CANVAS_CENTER_H),
                     lcd(TFT_CS, TFT_DC, TFT_RST),
                     ts(STMPE_CS)
{
}

void Display::initDisplay()
{
    // LCD SETUP
    lcd.begin(12000000UL);
    lcd.fillScreen(HX8357_BLACK); // initialize the lcd
    lcd.setRotation(0);
    lcd.setTextSize(1);

    // Start Touch Screen
    ts.begin();

    cCenter.fillScreen(HX8357_BLACK);
    cCenter.setFont(&B612_Bold16pt7b);
    cCenter.setCursor(5, 50);
    cCenter.print(F("Initializing"));
    lcd.drawBitmap(CENTER_LABEL_POS_X, CENTER_LABEL_POS_Y, cCenter.getBuffer(), CANVAS_CENTER_W, CANVAS_CENTER_H, HX8357_WHITE);
}

TouchEvent Display::processTouch()
{
    int8_t nav_sel = -1;
    int8_t crs_sel = -1;

    if (!ts.bufferEmpty())
    {
        while (!ts.bufferEmpty())
        {
            TS_Point p = ts.getPoint();
            p.x = map(p.x, TS_MINX, TS_MAXX, 0, lcd.width());
            p.y = map(p.y, TS_MINY, TS_MAXY, 0, lcd.height());

            if (p.y > RADIO_BUTTON_POS_Y && p.y < (RADIO_BUTTON_POS_Y + RADIO_BUTTON_H))
            {
                for (int i = 0; i < 5; i++)
                {
                    if (p.x > (15 + (RADIO_BUTTON_V + 10) * i) && p.x < (5 + (RADIO_BUTTON_V + 10) * (i + 1)))
                    {
                        lcd.drawCircle(p.x, p.y, 1, HX8357_CYAN);
                        nav_sel = i;
                    }
                }
            } else if (p.y > 260 && p.y < 325 && p.x > 160) {
                lcd.drawCircle(p.x, p.y, 1, HX8357_CYAN);
                crs_sel = 1;
            }

        }
        if (nav_sel > -1 && (nav_sel != radio.sel))
        {
            radio.sel = nav_sel;
            update.radio_buttons = true;
            TouchEvent te;
            te.event = TouchEventType::NAV_BUTTON;
            te.value = nav_sel;
            return te;
        } else if (crs_sel > -1) {
            nav_data.crs_sel = (nav_data.crs_sel + 1) % 2;
            update.crs = true;
            TouchEvent te;
            te.event = TouchEventType::CRS_BUTTON;
            te.value = nav_data.crs_sel;
            return te;
        } else {
            TouchEvent te;
            return te;            
        }
    }
    else
    {
        TouchEvent te;
        return te;
    }
}

void Display::startConfig()
{
    lcd.fillScreen(HX8357_BLACK);
    cCenter.fillScreen(HX8357_BLACK);
    cCenter.setFont(&B612_Bold16pt7b);
    cCenter.setCursor(5, 50);
    cCenter.print(F("Configuring"));
    lcd.drawBitmap(CENTER_LABEL_POS_X, CENTER_LABEL_POS_Y, cCenter.getBuffer(), CANVAS_CENTER_W, CANVAS_CENTER_H, HX8357_WHITE);
}

void Display::printStatic()
{
    lcd.fillScreen(HX8357_BLACK); // initialize the lcd

    lcd.setTextColor(HX8357_GREEN);
    lcd.setFont(&B612_Regular10pt7b);
    lcd.setCursor(5, 20);
    lcd.print(F("Active"));
    lcd.setCursor(165, 20);
    lcd.print(F("Standby"));

    lcd.drawFastHLine(0, 145, 320, HX8357_WHITE);

    lcd.setCursor(5, 175);
    lcd.print(F("Altitude"));
    lcd.setCursor(165, 175);
    lcd.print(F("VS"));

    lcd.drawFastHLine(0, 230, 320, HX8357_WHITE);

    lcd.setCursor(5, 260);
    lcd.print(F("Heading"));
    lcd.setCursor(165, 260);
    lcd.print(F("Course 1 "));

    lcd.drawFastHLine(0, 325, 320, HX8357_WHITE);

    lcd.setCursor(5, 355);
    lcd.print(F("XPDR"));
    lcd.setCursor(165, 355);
    lcd.print(F("Barometer"));

    lcd.drawFastHLine(0, 410, 320, HX8357_WHITE);
}

void Display::printButtons(uint8_t active)
{

    uint16_t colors[5] = {HX8357_CUST_GRAY, HX8357_CUST_GRAY, HX8357_CUST_GRAY, HX8357_CUST_GRAY, HX8357_CUST_GRAY};
    colors[active] = HX8357_WHITE;

    lcd.setTextColor(HX8357_BLACK);
    lcd.setFont(&B612_Bold7pt7b);

    uint8_t button_pos_v = 0;
    for (int i = 0; i < 5; i++)
    {
        button_pos_v = 15 + (RADIO_BUTTON_V + 10) * i;
        lcd.fillRoundRect(button_pos_v, RADIO_BUTTON_POS_Y, RADIO_BUTTON_V, RADIO_BUTTON_H, RADIO_BUTTON_RAD, colors[i]);
        lcd.setCursor(button_pos_v + 5, RADIO_BUTTON_TXTPOS_Y);
        lcd.print(F(RADIO_BUTTON_LABEL[i]));
    }

    update.radio_buttons = false;
}

void Display::redraw()
{
    if (update.alt)
        drawAltitude();
    if (update.hdg)
        drawHeading();
    if (update.crs)
        drawCourse();
    if (update.vs)
        drawVerticalSpeed();
    if (update.xpdr)
        drawTransponderCode();
    if (update.radio_buttons)
        printButtons(radio.sel);
    if (update.radio_active)
        drawRadioActive();
    if (update.radio_standby)
        drawRadioStandby();
    if (update.baro)
        drawBarometer();
}

void Display::setAltitude(int32_t alt)
{
    nav_data.alt = alt;
    update.alt = true;
}

void Display::drawAltitude()
{
    cNum.fillScreen(HX8357_BLACK);
    cNum.setFont(&B612Mono_Bold18pt7b);
    cNum.setCursor(5, 35);
    cNum.printf("%5i", nav_data.alt);
    lcd.drawBitmap(DATA_ALT_POS_X, DATA_ALT_POS_Y, cNum.getBuffer(), CANVAS_NUM_W, CANVAS_NUM_H, HX8357_WHITE, HX8357_BLACK);
    update.alt = false;
}

void Display::setVerticalSpeed(int16_t vs)
{
    nav_data.vs = vs;
    update.vs = true;
}

void Display::drawVerticalSpeed()
{
    cNum.fillScreen(HX8357_BLACK);
    cNum.setFont(&B612Mono_Bold18pt7b);
    cNum.setCursor(5, 35);
    cNum.printf("%5i", nav_data.vs);
    lcd.drawBitmap(DATA_VS_POS_X, DATA_VS_POS_Y, cNum.getBuffer(), CANVAS_NUM_W, CANVAS_NUM_H, HX8357_CUST_GRAY, HX8357_BLACK);
    update.vs = false;
}

void Display::setHeading(int16_t hdg)
{
    if (hdg == 0)
        nav_data.hdg = 360;
    else
        nav_data.hdg = hdg;

    update.hdg = true;
}

void Display::drawHeading()
{
    cNumLarge.fillScreen(HX8357_BLACK);
    cNumLarge.setFont(&B612Mono_Bold24pt7b);
    cNumLarge.setCursor(5, 45);
    cNumLarge.printf("%03i", nav_data.hdg);
    lcd.drawBitmap(DATA_HDG_POS_X, DATA_HDG_POS_Y, cNumLarge.getBuffer(), CANVAS_NUM_LARGE_W, CANVAS_NUM_LARGE_H, HX8357_WHITE, HX8357_BLACK);
    update.hdg = false;
}

void Display::setCourse(int16_t crs)
{
    if (crs == 0)
        nav_data.crs = 360;
    else
        nav_data.crs = crs;
    
    update.crs = true;
}

void Display::updateCourseLabel(uint8_t selection) 
{
    lcd.setFont(&B612_Regular10pt7b);
    lcd.setTextColor(HX8357_BLACK);
    lcd.setCursor(165, 260);
    lcd.printf("Course %i ", ((selection + 1) % 2 + 1));

    lcd.setTextColor(HX8357_GREEN);
    lcd.setCursor(165, 260);
    lcd.printf("Course %i ", (selection + 1));

}

void Display::drawCourse()
{
    cNumLarge.fillScreen(HX8357_BLACK);
    cNumLarge.setFont(&B612Mono_Bold24pt7b);
    cNumLarge.setCursor(5, 45);
    cNumLarge.printf("%03i", nav_data.crs);
    lcd.drawBitmap(DATA_CRS_POS_X, DATA_CRS_POS_Y, cNumLarge.getBuffer(), CANVAS_NUM_LARGE_W, CANVAS_NUM_LARGE_H, HX8357_WHITE, HX8357_BLACK);
    update.crs = false;
}

void Display::setTransponderCode(int16_t xpdr)
{
    radio.xpdr = xpdr;
    update.xpdr = true;
}

void Display::drawTransponderCode()
{
    cNum.fillScreen(HX8357_BLACK);
    cNum.setFont(&B612Mono_Bold18pt7b);
    cNum.setCursor(5, 35);
    cNum.printf("%04i", radio.xpdr);
    lcd.drawBitmap(DATA_XPDR_POS_X, DATA_XPDR_POS_Y, cNum.getBuffer(), CANVAS_NUM_W, CANVAS_NUM_H, HX8357_WHITE, HX8357_BLACK);
    update.xpdr = false;
}

void Display::setBarometer(float_t baro)
{
    nav_data.baro = baro;
    update.baro = true;
}

void Display::drawBarometer()
{
    cNum.fillScreen(HX8357_BLACK);
    cNum.setFont(&B612Mono_Bold18pt7b);
    cNum.setCursor(5, 35);
    cNum.printf("%2.2f", nav_data.baro);
    lcd.drawBitmap(DATA_BARO_POS_X, DATA_BARO_POS_Y, cNum.getBuffer(), CANVAS_NUM_W, CANVAS_NUM_H, HX8357_WHITE, HX8357_BLACK);
    update.baro = false;
}

void Display::setActiveRadio(uint8_t sel)
{
    radio.sel = sel;
    update.radio_active = true;
    update.radio_standby = true;
    update.radio_buttons = true;
}

void Display::setRadioFrequencyActive(float_t freq)
{
    radio.freq.active = freq;
    update.radio_active = true;
}

void Display::setRadioFrequencyStandby(float_t freq)
{
    radio.freq.standby = freq;
    update.radio_standby = true;
}

void Display::drawRadioActive()
{
    cNum.fillScreen(HX8357_BLACK);
    cNum.setFont(&B612Mono_Bold18pt7b);
    cNum.setCursor(5, 35);
    cNum.printf("%3.2f", radio.freq.active);
    lcd.drawBitmap(DATA_RADIO_ACT_POS_X, DATA_RADIO_ACT_POS_Y, cNum.getBuffer(), CANVAS_NUM_W, CANVAS_NUM_H, HX8357_WHITE, HX8357_BLACK);
    update.radio_active = false;
}

void Display::drawRadioStandby()
{
    cNum.fillScreen(HX8357_BLACK);
    cNum.setFont(&B612Mono_Bold18pt7b);
    cNum.setCursor(5, 35);
    cNum.printf("%3.2f", radio.freq.standby);
    lcd.drawBitmap(DATA_RADIO_STANDBY_POS_X, DATA_RADIO_STANDBY_POS_Y, cNum.getBuffer(), CANVAS_NUM_W, CANVAS_NUM_H, HX8357_WHITE, HX8357_BLACK);
    update.radio_standby = false;
}

void Display::printSample()
{

    lcd.setTextColor(HX8357_WHITE);
    lcd.setFont(&B612Mono_Bold18pt7b);
    lcd.setCursor(5, 65);
    lcd.print(F("109.90"));
    lcd.setTextColor(0x9CD3);
    lcd.setCursor(165, 65);
    lcd.print(F("115.75"));

    lcd.setTextColor(HX8357_WHITE);
    lcd.setFont(&B612Mono_Bold18pt7b);
    lcd.setCursor(5, 215);
    lcd.print(F("22.500"));
    lcd.setTextColor(0x9CD3);
    lcd.setCursor(165, 215);
    lcd.print(F("-1000"));

    lcd.setTextColor(HX8357_WHITE);
    lcd.setFont(&B612Mono_Bold24pt7b);
    lcd.setCursor(5, 310);
    lcd.print(F("159"));
    lcd.setTextColor(HX8357_WHITE);
    lcd.setCursor(165, 310);
    lcd.print(F("165"));

    lcd.setTextColor(HX8357_WHITE);
    lcd.setFont(&B612Mono_Bold18pt7b);
    // lcd.setCursor(5, 395);
    // lcd.print(F("22.500"));
    lcd.setTextColor(HX8357_WHITE);
    lcd.setCursor(165, 395);
    lcd.print(F("22.95"));

    lcd.setFont(&B612_Regular10pt7b);
    lcd.setCursor(5, 450);
    lcd.print(F("Next waypoint in 10s"));
}

void Display::lastCommand(uint8_t command, int32_t val) {

    lcd.setFont(NULL);
    lcd.setCursor(5, 450);
    lcd.printf("Cmd %2d %d", command, val);

}