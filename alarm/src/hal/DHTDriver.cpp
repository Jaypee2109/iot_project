#include "DHTDriver.h"

DHTDriver::DHTDriver(const char* ssid, const char* password,
                     const char* server, const char* path, uint16_t port) 
  : _ssid(ssid), _password(password), _server(server), _path(path), _port(port) {

}

void DHTDriver::begin() {
  // Start I2C for the DHT20 sensor.
  Wire.begin();
  // Initialize Wi-Fi connection.
  connectWiFi();
  
  // Begin the sensor.
  _dht20.begin();
  Serial.println("DHT20 sensor initialized");
}

bool DHTDriver::connectWiFi() {
  Serial.printf("Connecting to WiFi: %s\n", _ssid);
  WiFi.begin(_ssid, _password);
  unsigned long startAttemptTime = millis();

  // Keep trying for up to 30 seconds
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 30000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.println("WiFi connection failed");
    return false;
  }
}

bool DHTDriver::readSensor() {
  uint8_t status = _dht20.read();
  if (status != 0) {
    Serial.print("DHT20 read failed, status code = ");
    Serial.println(status);
    return false;
  }

  // Optionally check for NaN values.
  float temperature = _dht20.getTemperature();
  float humidity = _dht20.getHumidity();
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT20 returned NaN values!");
    return false;
  }
  return true;
}

float DHTDriver::getTemperature() {
  return _dht20.getTemperature();
}

float DHTDriver::getHumidity() {
  return _dht20.getHumidity();
}

bool DHTDriver::sendData() {
  // Construct the URL with query parameters.
  float temperature = getTemperature();
  float humidity = getHumidity();
  
  String url = String(_path) + 
               "?temperature=" + String(temperature) +
               "&humidity=" + String(humidity);

  WiFiClient client;
  HttpClient http(client);

  Serial.println("Sending HTTP GET request...");
  int err = http.get(_server, 5000, url.c_str());

  if (err != 0) {
    Serial.print("Failed to initiate HTTP request, error: ");
    Serial.println(err);
    return false;
  }

  // Get and print the response status.
  err = http.responseStatusCode();
  if (err < 0) {
    Serial.print("Getting response failed, error: ");
    Serial.println(err);
    return false;
  }
  Serial.print("Response Status: ");
  Serial.println(err);

  // Skip HTTP response headers.
  err = http.skipResponseHeaders();
  if (err < 0) {
    Serial.print("Failed to skip response headers, error: ");
    Serial.println(err);
    return false;
  }

  Serial.println("HTTP Response Body:");
  unsigned long timeoutStart = millis();
  while ((http.connected() || http.available()) && (millis() - timeoutStart < 30000)) {
    if (http.available()) {
      char c = http.read();
      Serial.print(c);
      timeoutStart = millis(); // reset timeout after each available byte
    } else {
      delay(1000);
    }
  }
  Serial.println("\n== HTTP Response Received ==");

  http.stop();
  return true;
}
