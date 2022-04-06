#ifndef LIGHTS_H
#define LIGHTS_H

#include "Arduino.h"

#define PIN_AP 18
#define PIN_HDG 17
#define PIN_NAV 16
#define PIN_ALT 15
#define PIN_VS 11
#define PIN_APR 12
#define PIN_BACKLIGHT 13

#define LIGHT_BRIGHT 192
#define LIGHT_DIM 8
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
        void updatePinToStyle(uint8_t pin, LightStyle style);

    public:
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