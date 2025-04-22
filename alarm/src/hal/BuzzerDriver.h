#ifndef BUZZERDRIVER_H
#define BUZZERDRIVER_H

#include <Arduino.h>

/**
 * BuzzerDriver provides a simple interface to trigger an alarm notification.
 * 
 * Instead of running continuously, call notify() when you want to produce a
 * single beep (or tone) for a specified duration.
 */
class BuzzerDriver {
  public:
    /**
     * Constructs a BuzzerDriver for the given pin and LEDC channel.
     * @param pin The GPIO pin connected to the buzzer.
     * @param channel The LEDC channel to use.
     */
    BuzzerDriver(uint8_t pin, uint8_t channel);

    /**
     * Initializes the buzzer (attaches the pin and sets up the LEDC channel).
     */
    void begin();

    /**
     * Triggers an alarm notification beep.
     * This function will immediately play the tone at the given frequency and volume
     * for the specified duration (in milliseconds), then turn off the buzzer.
     *
     * @param frequency Tone frequency in Hz.
     * @param volume LEDC duty value (0 to 255; higher values produce a louder tone).
     * @param duration Duration (in milliseconds) to sound the tone.
     */
    void notify(uint32_t frequency, uint8_t volume, unsigned long duration);

  private:
    uint8_t _pin;
    uint8_t _channel;
};

#endif
