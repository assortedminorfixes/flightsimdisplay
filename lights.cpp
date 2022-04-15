#include "lights.hh"
#include "Arduino.h"

LightController::LightController() : strip(6, PIXEL_PIN, NEO_GRBW + NEO_KHZ800)
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

void LightController::updatePinToStyle(uint8_t pixel, LightStyle style)
{
    switch (style)
    {
    case LightStyle::OFF:
        strip.setPixelColor(pixel, strip.Color(0, 0, 0, 0));
        break;
    case LightStyle::DIM:
        strip.setPixelColor(pixel, strip.Color(0, LIGHT_DIM, 0, 0));
        break;
    case LightStyle::BRIGHT:
        strip.setPixelColor(pixel, strip.Color(0, LIGHT_BRIGHT, 0, 0));
        break;
    }
    strip.show();
}

void LightController::update()
{
    return;
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