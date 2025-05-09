#ifndef DHTDRIVER_H
#define DHTDRIVER_H

#include <Arduino.h>
#include <Wire.h>
#include <DHT20.h>

/**
 * DHTDriver wraps the DHT20 temperature/humidity sensor.
 */
class DHTDriver {
public:
  /** Initialize pins and the sensor. */
  void begin();

  /** Trigger a read; returns false on error. */
  bool read();

  /** Last read temperature in °C. */
  float getTemperature();

  /** Last read humidity in %. */
  float getHumidity();

private:
  DHT20 _sensor;
};

#endif
