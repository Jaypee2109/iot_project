#include "core/AlarmConfig.h"
#include "hal/WifiModule.h"
#include <ArduinoJson.h>

extern WifiModule wifi;

AlarmConfig::AlarmConfig(AlarmScheduler& scheduler,
                         const char* serverHost,
                         uint16_t serverPort,
                         const char* endpointPath,
                         unsigned long refreshPeriod)
  : _scheduler(scheduler)
  , _host(serverHost)
  , _port(serverPort)
  , _path(endpointPath)
  , _period(refreshPeriod)
  , _lastFetch(0)
{}

void AlarmConfig::begin() {
  // Do an immediate fetch so we have an alarm in place right away.
  if (fetchAlarm()) {
    Serial.println("Initial alarm fetched.");
  } else {
    Serial.println("Initial alarm fetch failed.");
  }
}

void AlarmConfig::update() {
  unsigned long now = millis();
  if (now - _lastFetch >= _period) {
    _lastFetch = now;
    if (fetchAlarm()) {
      Serial.println("Alarm re-fetched successfully.");
    } else {
      Serial.println("Alarm re-fetch failed.");
    }
  }
}
bool AlarmConfig::fetchAlarm() {
  Serial.println("Fetching remote alarm…");

  // 1) Perform HTTP GET via WifiModule
  String body;
  int status = wifi.httpGet(_host, _port, _path, body);
  if (status != 200) {
    Serial.printf("Failed to fetch alarm: HTTP %d\n", status);
    return false;
  }

  // 2) Parse JSON response
  StaticJsonDocument<128> doc;
  auto err = deserializeJson(doc, body);
  if (err) {
    Serial.print("JSON parse failed: ");
    Serial.println(err.c_str());
    return false;
  }

  // 3) Extract hour & minute and apply
  uint8_t h = doc["hour"];
  uint8_t m = doc["minute"];
  Serial.printf("Received alarm %02u:%02u\n", h, m);
  _scheduler.setAlarm(h, m);
  return true;
}