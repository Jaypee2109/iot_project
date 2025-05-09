#include "TimeSync.h"

TimeSync::TimeSync(const char* ntpServer1, const char* ntpServer2, long gmtOffsetSec, int daylightOffsetSec, int maxRetries)
  : _ntpServer1(ntpServer1), _ntpServer2(ntpServer2),
    _gmtOffsetSec(gmtOffsetSec), _daylightOffsetSec(daylightOffsetSec), _maxRetries(maxRetries)
{
}

void TimeSync::begin() {
  // Configure NTP using the ESP32 built-in SNTP client.
  configTime(_gmtOffsetSec, _daylightOffsetSec, _ntpServer1, _ntpServer2);
}

bool TimeSync::sync() {
  struct tm timeinfo;
  int retry = 0;
  // Wait until getLocalTime() returns true or we hit the maximum retry count.
  while (!getLocalTime(&timeinfo) && (retry < _maxRetries)) {
    Serial.println("Waiting for time synchronization...");
    delay(2000);
    retry++;
  }

  if (retry >= _maxRetries) {
    Serial.println("Failed to obtain time");
    return false;
  }

  // Print the synchronized time.
  Serial.println(&timeinfo, "Time synchronized: %A, %B %d %Y %H:%M:%S");
  return true;
}

String TimeSync::getFormattedTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "Time not set";
  }
  char buffer[64];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buffer);
}

time_t TimeSync::getEpochTime() {
  // Returns the current epoch time.
  return time(nullptr);
}
