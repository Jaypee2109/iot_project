#ifndef LEDDriver_H
#define LEDDriver_H

#include <Arduino.h>

class LEDDriver {
  public:
    LEDDriver(std::initializer_list<uint8_t> pins);
    void begin();
    void setPattern(const bool pattern[]);
    void clear();
  
  private:
    uint8_t _numPins;
    uint8_t *_pins;
};

#endif
