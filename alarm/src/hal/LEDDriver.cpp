#include "LEDDriver.h"

LEDDriver::LEDDriver(std::initializer_list<uint8_t> pins) {
  _numPins = pins.size();
  _pins = new uint8_t[_numPins];
  uint8_t i = 0;
  for (auto pin : pins) {
    _pins[i++] = pin;
  }
}

void LEDDriver::begin() {
  for (uint8_t i = 0; i < _numPins; i++) {
    pinMode(_pins[i], OUTPUT);
    digitalWrite(_pins[i], LOW);
  }
}

void LEDDriver::setPattern(const bool pattern[]) {
  for (uint8_t i = 0; i < _numPins; i++) {
    digitalWrite(_pins[i], pattern[i] ? HIGH : LOW);
  }
}

void LEDDriver::clear() {
  for (uint8_t i = 0; i < _numPins; i++) {
    digitalWrite(_pins[i], LOW);
  }
}
