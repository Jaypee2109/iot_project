#ifndef ALARMSCHEDULER_H
#define ALARMSCHEDULER_H

#include <Arduino.h>
#include <time.h>

// Define the type for the alarm callback function.
typedef void (*AlarmCallback)();

class AlarmScheduler {
  public:
    AlarmScheduler();

    /**
     * Sets the alarm time in 24-hour format.
     * @param hour   Hour (0-23).
     * @param minute Minute (0-59).
     */
    void setAlarm(uint8_t hour, uint8_t minute);

    /**
     * Sets the callback function that will be called when the alarm is triggered.
     * @param callback The function to call on alarm trigger.
     */
    void setCallback(AlarmCallback callback);

    /**
     * Checks the current time; if it matches the alarm time and hasn’t been triggered 
     * this minute, the callback is called.
     */
    void checkAlarm();

  private:
    uint8_t _alarmHour;
    uint8_t _alarmMinute;
    bool _alarmSet;
    bool _alarmTriggered;  // Ensure we trigger only once per alarm minute.
    AlarmCallback _callback;
};

#endif
