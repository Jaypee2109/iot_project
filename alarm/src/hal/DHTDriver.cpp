#include "DHTDriver.h"

void DHTDriver::begin() {
  // Initialize default pins
  Wire.begin();
  // Initialize the DHT20
  if (!_sensor.begin()) {
    Serial.println("DHT20 init failed!");
    while (1) delay(1000);
  }
  Serial.println("DHT20 initialized.");
}

bool DHTDriver::read() {
  uint8_t status = _sensor.read();
  if (status != 0) {
    Serial.printf("DHT20 read failed: %u\n", status);
    return false;
  }
  float t = _sensor.getTemperature();
  float h = _sensor.getHumidity();
  if (isnan(t) || isnan(h)) {
    Serial.println("DHT20 returned NaN.");
    return false;
  }
  return true;
}

float DHTDriver::getTemperature() {
  return _sensor.getTemperature();
}

float DHTDriver::getHumidity() {
  return _sensor.getHumidity();
}
