#ifndef LIGHTS_H
#define LIGHTS_H

#include "Arduino.h"
#include "Adafruit_NeoPixel.h"

#define PIXEL_PIN 14
#define PIXEL_AP 0
#define PIXEL_HDG 1
#define PIXEL_NAV 2
#define PIXEL_ALT 3
#define PIXEL_VS 4
#define PIXEL_APR 5

#define LIGHT_BRIGHT 128
#define LIGHT_DIM 4
#define LIGHT_BACKLIGHT 16
#define LIGHT_OFF 0

enum LightStyle {
   OFF,
   DIM,
   BRIGHT,
   BLINK
};

class LightController {
    
    private:
        Adafruit_NeoPixel strip;
        void colorWipe(uint32_t c, uint8_t wait);
        void updatePinToStyle(uint8_t pixel, LightStyle style);

    public:
        LightController();
        void initLights();
        void update();
        void setAutopilot(LightStyle style);
        void setAutopilot(bool on);
        void setHeading(LightStyle style);
        void setHeading(bool on);
        void setNavigation(LightStyle style);
        void setNavigation(bool on);                
        void setAltitude(LightStyle style);
        void setAltitude(bool on);
        void setVerticalSpeed(LightStyle style);
        void setVerticalSpeed(bool on);
        void setApproach(LightStyle style);
        void setApproach(bool on);        
};

extern LightController lights;

#endif