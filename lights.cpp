#include "lights.hh"
#include "Arduino.h"

LightController::LightController() : strip(PIXEL_NUM, PIXEL_PIN, NEO_GRBW + NEO_KHZ800)
{

    strip.begin();
    strip.setBrightness(125);
    strip.show(); // Initialize all pixels to 'off'
}

void LightController::initLights()
{
    this->colorWipe(strip.Color(0, 0, 0, LIGHT_BACKLIGHT), 50);
    strip.clear();
    strip.show();
}

void LightController::setAutopilot(LightStyle style)
{
    this->updatePinToStyle(PIXEL_AP, style);
}

void LightController::setAutopilot(bool on)
{
    if (on)
        this->setAutopilot(LightStyle::BRIGHT);
    else
        this->setAutopilot(LightStyle::DIM);
}

void LightController::setHeading(LightStyle style)
{
    this->updatePinToStyle(PIXEL_HDG, style);
}

void LightController::setHeading(bool on)
{
    if (on)
        this->setHeading(LightStyle::BRIGHT);
    else
        this->setHeading(LightStyle::DIM);
}

void LightController::setNavigation(LightState state)
{
    this->updatePinToStyle(PIXEL_NAV, state);
}

void LightController::setNavigation(LightStyle style)
{
    this->updatePinToStyle(PIXEL_NAV, style);
}

void LightController::setNavigation(bool on)
{
    if (on)
        this->setNavigation(LightStyle::BRIGHT);
    else
        this->setNavigation(LightStyle::DIM);
}

void LightController::setAltitude(LightStyle style)
{
    this->updatePinToStyle(PIXEL_ALT, style);
}

void LightController::setAltitude(bool on)
{
    if (on)
        this->setAltitude(LightStyle::BRIGHT);
    else
        this->setAltitude(LightStyle::DIM);
}

void LightController::setVerticalSpeed(LightStyle style)
{
    this->updatePinToStyle(PIXEL_VS, style);
}

void LightController::setVerticalSpeed(bool on)
{
    if (on)
        this->setVerticalSpeed(LightStyle::BRIGHT);
    else
        this->setVerticalSpeed(LightStyle::DIM);
}

void LightController::setApproach(LightStyle style)
{
    this->updatePinToStyle(PIXEL_APR, style);
}

void LightController::setApproach(bool on)
{
    if (on)
        this->setApproach(LightStyle::BRIGHT);
    else
        this->setApproach(LightStyle::DIM);
}

void LightController::updatePinToStyle(uint8_t pixel, LightState state)
{
    pixelState[pixel] = state;
}

void LightController::updatePinToStyle(uint8_t pixel, LightStyle style)
{
    pixelState[pixel].style = style;
    pixelState[pixel].color = LightColor::GREEN;
}

uint32_t LightController::convertStateToColor(LightState state) 
{
    
    uint32_t color = 0;
    
    switch (state.color) {
        case LightColor::GREEN:
            color = strip.Color(0, state.style, 0, 0);
            break;
        case LightColor::YELLOW:
            color = strip.Color(state.style, state.style, 0, 0);
            break;
        case LightColor::WHITE:
            color = strip.Color(0, 0, 0, state.style);        
            break;
    }

    return color;

}

void LightController::update()
{
    for (int p = 0; p < PIXEL_NUM; p++) {
            strip.setPixelColor(p, convertStateToColor(pixelState[p]));
    }
    strip.show();
}

// Fill the dots one after the other with a color
void LightController::colorWipe(uint32_t c, uint8_t wait)
{
    for (uint16_t i = 0; i < strip.numPixels(); i++)
    {
        strip.setPixelColor(i, c);
        strip.show();
        delay(wait);
    }
}