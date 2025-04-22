#include "BuzzerDriver.h"

BuzzerDriver::BuzzerDriver(uint8_t pin, uint8_t channel)
  : _pin(pin), _channel(channel) {
  // Empty constructor body.
}

void BuzzerDriver::begin() {
  // Attach the buzzer pin to the specified LEDC channel.
  ledcAttachPin(_pin, _channel);
  // Setup LEDC channel with a default frequency and 8-bit resolution.
  ledcSetup(_channel, 5000, 8);
  // Ensure the buzzer is off at startup.
  ledcWriteTone(_channel, 0);
}

void BuzzerDriver::notify(uint32_t frequency, uint8_t volume, unsigned long duration) {
  // Start the tone at the specified frequency.
  ledcWriteTone(_channel, frequency);
  // Set the volume (duty cycle).
  ledcWrite(_channel, volume);
  // Keep the tone on for the given duration (blocking delay).
  delay(duration);
  // Turn off the tone.
  ledcWriteTone(_channel, 0);
}
