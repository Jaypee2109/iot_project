#ifndef TIMESYNC_H
#define TIMESYNC_H

#include <Arduino.h>
#include <time.h>

/**
 * TimeManager configures the ESP32’s internal NTP client and provides methods to obtain the current time.
 * 
 * This module encapsulates the use of configTime() and getLocalTime() so that the application
 * can easily wait for time synchronization and then retrieve the current time in different formats.
 */
class TimeSync {
  public:
    /**
     * Constructs a TimeManager with given NTP server settings and time offsets.
     * @param ntpServer1        Primary NTP server.
     * @param ntpServer2        Secondary NTP server.
     * @param gmtOffsetSec      GMT offset in seconds.
     * @param daylightOffsetSec Daylight saving offset in seconds.
     * @param maxRetries        Maximum number of retries to wait for synchronization (default is 10).
     */
    TimeSync(const char* ntpServer1, const char* ntpServer2, long gmtOffsetSec, int daylightOffsetSec, int maxRetries = 10);

    /**
     * Begins time synchronization by calling configTime() with the provided settings.
     */
    void begin();

    /**
     * Waits (blocking) until the local time is synchronized or until maxRetries is reached.
     * @return true if time synchronization was successful, false otherwise.
     */
    bool sync();

    /**
     * Retrieves the current local time as a formatted string.
     * @return A String formatted as "YYYY-MM-DD HH:MM:SS"; returns "Time not set" if not synchronized.
     */
    String getFormattedTime();

    /**
     * Retrieves the current epoch time.
     * @return A time_t value representing the current epoch time.
     */
    time_t getEpochTime();

  private:
    const char* _ntpServer1;
    const char* _ntpServer2;
    long _gmtOffsetSec;
    int _daylightOffsetSec;
    int _maxRetries;
};

#endif
