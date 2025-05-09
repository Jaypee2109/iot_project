#include "AlarmScheduler.h"

AlarmScheduler::AlarmScheduler() : _alarmSet(false), _alarmTriggered(false), _callback(nullptr) {
}

void AlarmScheduler::setAlarm(uint8_t hour, uint8_t minute) {
  _alarmHour = hour;
  _alarmMinute = minute;
  _alarmSet = true;
  _alarmTriggered = false;
  Serial.print("Alarm set for ");
  Serial.print(hour);
  Serial.print(":");
  Serial.println(minute);
}

void AlarmScheduler::setCallback(AlarmCallback callback) {
  _callback = callback;
}

void AlarmScheduler::checkAlarm() {
  if (!_alarmSet)
    return; // No alarm has been set.

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  uint8_t currentHour = timeinfo.tm_hour;
  uint8_t currentMinute = timeinfo.tm_min;

  // If the current time equals the alarm time
  if ((currentHour == _alarmHour) && (currentMinute == _alarmMinute)) {
    // and haven’t yet triggered an alarm during the current minute
    if (!_alarmTriggered) {
      Serial.println("Alarm triggered!");
      _alarmTriggered = true;
      if (_callback) {
        _callback();
      }
    }
  } else {
    // Reset trigger flag when the alarm minute has passed.
    _alarmTriggered = false;
  }
}
