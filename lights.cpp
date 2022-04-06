#include "lights.hh"
#include "Arduino.h"

void LightController::initLights() {
  pinMode(PIN_AP, OUTPUT);
  pinMode(PIN_HDG, OUTPUT);
  pinMode(PIN_NAV, OUTPUT);
  pinMode(PIN_ALT, OUTPUT);
  pinMode(PIN_VS, OUTPUT);
  pinMode(PIN_APR, OUTPUT);
  pinMode(PIN_BACKLIGHT, OUTPUT);
  analogWrite(PIN_BACKLIGHT, LIGHT_BACKLIGHT);
}

void LightController::setAutopilot(LightStyle style) {
    this->updatePinToStyle(PIN_AP, style);
}

void LightController::setAutopilot(bool on) {
    if (on)
        this->setAutopilot(LightStyle::BRIGHT);
    else
        this->setAutopilot(LightStyle::DIM);
}

void LightController::setHeading(LightStyle style) {
    this->updatePinToStyle(PIN_HDG, style);
}

void LightController::setHeading(bool on) {
    if (on)
        this->setHeading(LightStyle::BRIGHT);
    else
        this->setHeading(LightStyle::DIM);
}

void LightController::setNavigation(LightStyle style) {
    this->updatePinToStyle(PIN_NAV, style);
}

void LightController::setNavigation(bool on) {
    if (on)
        this->setNavigation(LightStyle::BRIGHT);
    else
        this->setNavigation(LightStyle::DIM);
}

void LightController::setAltitude(LightStyle style) {
    this->updatePinToStyle(PIN_ALT, style);
}

void LightController::setAltitude(bool on) {
    if (on)
        this->setAltitude(LightStyle::BRIGHT);
    else
        this->setAltitude(LightStyle::DIM);
}

void LightController::setVerticalSpeed(LightStyle style) {
    this->updatePinToStyle(PIN_VS, style);
}

void LightController::setVerticalSpeed(bool on) {
    if (on)
        this->setVerticalSpeed(LightStyle::BRIGHT);
    else
        this->setVerticalSpeed(LightStyle::DIM);
}

void LightController::setApproach(LightStyle style) {
    this->updatePinToStyle(PIN_APR, style);
}

void LightController::setApproach(bool on) {
    if (on)
        this->setApproach(LightStyle::BRIGHT);
    else
        this->setApproach(LightStyle::DIM);
}

void LightController::updatePinToStyle(uint8_t pin, LightStyle style) {
    switch (style) {
        case LightStyle::OFF:
            analogWrite(pin, LIGHT_OFF);
            break;
        case LightStyle::DIM:
            analogWrite(pin, LIGHT_DIM);
            break;
        case LightStyle::BRIGHT:
            analogWrite(pin, LIGHT_BRIGHT);
            break;
    }
}

void LightController::update() {
    return;
}