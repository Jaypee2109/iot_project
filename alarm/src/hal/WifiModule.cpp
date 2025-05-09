#include <hal/WifiModule.h>
#include <WiFi.h>
#include <HTTPClient.h>

WifiModule::WifiModule(const char* ssid, const char* password)
  : _ssid(ssid), _password(password) {}

bool WifiModule::begin(unsigned long timeoutMs) {
  WiFi.begin(_ssid, _password);
  unsigned long start = millis();
  Serial.printf("Connecting to Wi-Fi '%s'…\n", _ssid);
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > timeoutMs) {
      Serial.println("  → Timeout.");
      return false;
    }
    delay(500);
    Serial.print('.');
  }
  Serial.println("\nWi-Fi connected.");
  Serial.print("IP: "); Serial.println(WiFi.localIP());
  return true;
}

int WifiModule::httpGet(const char* host,
                       uint16_t port,
                       const char* path,
                       String& responseBody) {

  WiFiClient net;
  HTTPClient http;
  
  http.begin(net, host, port, path);
  
  int status = http.GET();
  if (status > 0) {
    responseBody = http.getString();
  } else {
    Serial.printf("GET failed, code=%d\n", status);
  }
  http.end();
  return status;
}

int WifiModule::httpPost(const char* host,
  uint16_t port,
  const char* path,
  const char* jsonPayload,
  String& responseBody) {

  WiFiClient net;
  HTTPClient http;
  
  http.begin(net, host, port, path);

  // Set content type
  http.addHeader("Content-Type", "application/json");
  String body = String(jsonPayload);
  // Send the POST
  int status = http.POST(body);

  if (status > 0) {
    // Read full response body
    responseBody = http.getString();
  } else {
    Serial.printf("POST failed, code=%d\n", status);
  }
  http.end();

  return status;
}
