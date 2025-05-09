#ifndef WIFIMODULE_H
#define WIFIMODULE_H

#include <Arduino.h>

class WifiModule {
public:
  WifiModule(const char* ssid, const char* password);

  bool begin(unsigned long timeoutMs = 30000);

  // Perform an HTTP GET; returns HTTP status or negative on error.
  int httpGet(const char* host, uint16_t port, const char* path, String& responseBody);

  // Perform an HTTP POST with a JSON payload; returns HTTP status or negative on error.
  int httpPost(const char* host,
               uint16_t port,
               const char* path,
               const char* jsonPayload,
               String& responseBody);

private:
  const char* _ssid;
  const char* _password;
};

#endif
