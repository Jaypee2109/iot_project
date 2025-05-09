#ifndef ALARMCONFIG_H
#define ALARMCONFIG_H

#include <Arduino.h>
#include <WiFiClient.h>
#include <HttpClient.h>
#include <ArduinoJson.h>
#include <core/AlarmScheduler.h>

/**
 * AlarmConfig periodically fetches an alarm time (hour & minute) from
 * a REST endpoint and reprograms an AlarmScheduler.
 */
class AlarmConfig {
  public:
    /**
     * @param scheduler      Reference to your AlarmScheduler instance.
     * @param serverHost     The host (IP or domain) of your alarm‐config API.
     * @param serverPort     Port to connect to (80 or 443).
     * @param endpointPath   Full path (e.g. "/api/alarm").
     * @param refreshPeriod  How often (ms) to fetch a new alarm.
     */
    AlarmConfig(AlarmScheduler& scheduler,
                const char* serverHost,
                uint16_t serverPort,
                const char* endpointPath,
                unsigned long refreshPeriod = 60000);

    /** Call once in setup() after Wi-Fi is up and time is synced. */
    void begin();

    /** Call from loop() to do periodic fetch + re-set. */
    void update();

  private:
    AlarmScheduler& _scheduler;
    const char*     _host;
    uint16_t        _port;
    const char*     _path;
    unsigned long   _period;
    unsigned long   _lastFetch;
    bool            fetchAlarm();  // returns true if successfully fetched+set
};

#endif
