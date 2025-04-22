#ifndef BUTTONDRIVER_H
#define BUTTONDRIVER_H

#include <Arduino.h>
#include <initializer_list>

// Define the type for the button press callback function.
typedef void (*ButtonCallback)(uint8_t buttonPin);

class ButtonDriver {
  public:
    /**
     * Constructor that accepts an initializer list for button pins.
     * @param pins An initializer list of button pin numbers.
     * @param callback Function to be called when a button release (HIGH after LOW) is detected.
     * @param debounce_ms Debounce time in milliseconds (default is 200).
     */
    ButtonDriver(std::initializer_list<uint8_t> pins, ButtonCallback callback, unsigned long debounce_ms = 200);

    /**
     * Initializes the buttons (sets pin mode to INPUT_PULLUP).
     */
    void begin();

    /**
     * Polls each button pin. If a button's state goes from LOW to HIGH and passes debouncing,
     * calls the callback function.
     */
    void update();

    /**
     * Simulates a button press (for testing) by immediately calling the callback.
     * @param buttonPin The pin number to simulate the press for.
     */
    void simulateButtonPress(uint8_t buttonPin);

    /**
     * Checks whether any button is currently pressed.
     * @return true if at least one button is pressed; false otherwise.
     */
    bool isAnyButtonPressed();

  private:
    uint8_t _numPins;
    uint8_t* _pins;
    unsigned long* _lastPressed;  // for debouncing
    int* _prevState;              // store previous state for edge detection
    ButtonCallback _callback;
    unsigned long _debounce;  // Debounce time in milliseconds
};

#endif
