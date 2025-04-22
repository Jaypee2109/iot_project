#include "ButtonDriver.h"

ButtonDriver::ButtonDriver(std::initializer_list<uint8_t> pins, ButtonCallback callback, unsigned long debounce_ms)
  : _numPins(pins.size()), _callback(callback), _debounce(debounce_ms)
{
  // Allocate memory for pins, last-pressed times, and previous state.
  _pins = new uint8_t[_numPins];
  _lastPressed = new unsigned long[_numPins];
  _prevState = new int[_numPins];

  uint8_t index = 0;
  for (auto pin : pins) {
    _pins[index] = pin;
    _lastPressed[index] = 0;
    // Initialize previous state to HIGH (buttons with INPUT_PULLUP are normally HIGH).
    _prevState[index] = HIGH;
    index++;
  }
}

void ButtonDriver::begin() {
  // Set each button pin as INPUT_PULLUP (so they are HIGH when not pressed)
  for (uint8_t i = 0; i < _numPins; i++) {
    pinMode(_pins[i], INPUT);
  }
}

void ButtonDriver::update() {
  // Poll each button pin.
  for (uint8_t i = 0; i < _numPins; i++) {
    int currentState = digitalRead(_pins[i]);
    // Detect a rising edge: the button was previously LOW (pressed) and now is HIGH (released).
    if (_prevState[i] == LOW && currentState == HIGH) {
      unsigned long currentMillis = millis();
      // Only trigger if the debounce interval has elapsed.
      if (currentMillis - _lastPressed[i] > _debounce) {
        _lastPressed[i] = currentMillis;
        _callback(_pins[i]);
      }
    }
    // Update the previous state for this pin.
    _prevState[i] = currentState;
  }
}

void ButtonDriver::simulateButtonPress(uint8_t buttonPin) {
  // Directly call the callback for simulation/testing.
  _callback(buttonPin);
}

bool ButtonDriver::isAnyButtonPressed() {
  // Check if any button is currently pressed. For INPUT_PULLUP, a pressed button reads LOW.
  for (uint8_t i = 0; i < _numPins; i++) {
    if (digitalRead(_pins[i]) == LOW) {
      return true;
    }
  }
  return false;
}
