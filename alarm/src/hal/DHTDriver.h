#ifndef DHTDRIVER_H
#define DHTDRIVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <HttpClient.h>
#include <Wire.h>
#include <DHT20.h>

class DHTDriver {
  public:
    /**
     * Constructs a DHTDriver object with Wi-Fi and server parameters.
     * @param ssid        WiFi SSID.
     * @param password    WiFi password.
     * @param server      Server address (domain or IP).
     * @param path        URL path (endpoint) for sensor data.
     * @param port        Server port (default: 80).
     */
    DHTDriver(const char* ssid, const char* password,
              const char* server, const char* path, uint16_t port = 80);

    /// Initializes the I2C bus, sensor and connects Wi-Fi.
    void begin();

    /// Attempts to perform a Wi-Fi connection (blocking until connected).
    bool connectWiFi();

    /// Reads sensor values from the DHT20.
    bool readSensor();

    /// Returns the last measured temperature.
    float getTemperature();

    /// Returns the last measured humidity.
    float getHumidity();

    /// Sends sensor data via an HTTP GET request.
    bool sendData();

  private:
    const char* _ssid;
    const char* _password;
    const char* _server;
    const char* _path;
    uint16_t    _port;
    DHT20       _dht20;
};

#endif
