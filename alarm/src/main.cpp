#include <hal/LEDDriver.h>
#include <hal/DHTDriver.h>
#include <hal/BuzzerDriver.h>
#include <hal/ButtonDriver.h>
#include <hal/WifiModule.h>
#include <core/AlarmScheduler.h>
#include <core/PuzzleGame.h>
#include <core/TimeSync.h>
#include <credentials.h>
#include <core/AlarmConfig.h>


// Global Variables for Alarm Input
const uint8_t steps = 4;                  // Number of steps in the sequence
uint8_t correctSequence[steps] = {0};     // Holds the randomly generated correct pattern (LED indices)
uint8_t userSequence[steps] = {0};        // Holds the player's input (LED indices)
volatile uint8_t inputIndex = 0;          // Next free index for player input
volatile bool waitingForInput = false;    // Flag set when waiting for player's input
volatile bool alarmCancel = false;        // Flag set when a button cancels the alarm warning phase

// Server connection setup
const char* server = "18.188.56.179";
const char* sensorPath = "/api/sensor";
const char* metricsPath = "/api/metrics";

// AlarmScheduler and PuzzleGame modules
AlarmScheduler alarmScheduler;
PuzzleGame puzzle(4, 4, 3 , 1000);

// Button Callback
// This callback is triggered on a release edge
void onButtonPressed(uint8_t buttonPin) {
  Serial.print("Button pressed on pin: ");
  Serial.println(buttonPin);

  // If not waiting for player input, use the button to cancel the alarm warning
  if (!waitingForInput) {
    alarmCancel = true;
    Serial.println("Alarm cancellation triggered by button press.");
  }
  else {
    // Otherwise, record the player's input
    const uint8_t buttonMapping[steps] = {39, 38, 37, 36};
    uint8_t pressedIndex = 0;
    for (uint8_t i = 0; i < steps; i++) {
      if (buttonPin == buttonMapping[i]) {
        pressedIndex = i;
        break;
      }
    }
    if (inputIndex < steps) {
      userSequence[inputIndex] = pressedIndex;
      inputIndex++;
      Serial.print("Recorded input index: ");
      Serial.println(pressedIndex);
    }
  }
}

// AWS Connection config
AlarmConfig alarmConfig(
  alarmScheduler,
  server,           // Domain         
  5000,             // port
  "/api/alarm",     // endpoint path
  60UL * 1000UL     // refresh every 60 seconds
);

// LEDDriver setup (Assuming LED order: index 0: YELLOW, 1: BLUE, 2: RED, 3: GREEN)
const int led_YELLOW = 27;
const int led_BLUE   = 26;
const int led_RED    = 25;
const int led_GREEN  = 33;
LEDDriver ledDriver({led_YELLOW, led_BLUE, led_RED, led_GREEN});

// BuzzerDriver setup
const int buzzer = 15;
const int LEDC_CHANNEL = 0;
BuzzerDriver buzzerDriver(buzzer, LEDC_CHANNEL);

// ButtonDriver setup
ButtonDriver buttonDriver({39, 38, 37, 36}, onButtonPressed);

// DHTDriver setup
DHTDriver dhtDriver;

// Wifi setup
WifiModule wifi(WIFI_SSID, WIFI_PASS);

// TimeSync setup
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long gmtOffset_sec = -28800;      // California Standard Time (-8 hours from UTC)
const int daylightOffset_sec = 3600;    // DST adjustment: +1 hour (effective: -7 hours)
TimeSync timeManager(ntpServer1, ntpServer2, gmtOffset_sec, daylightOffset_sec);

unsigned long lastPostTime = 0;
const int interval = 300UL * 1000UL; // 5min

// Alarm Callback
// This function is invoked when the alarm time is reached
void alarmCallback() {
  static unsigned long lastFire = 0;
  unsigned long now = millis();
  // if fired in the last 60s, bail out
  if (now - lastFire < 60UL * 1000UL) return;
  // otherwise record this fire time
  lastFire = now;
  Serial.println("Alarm triggered!");

  // 1) Warning phase (buzz until button release)
  Serial.println("Warning: Buzz until a button release.");
  alarmCancel = false;
  while (!alarmCancel) {
    buzzerDriver.notify(500, 200, 1000);
    unsigned long start = millis();
    while (millis() - start < 1000 && !alarmCancel) {
      buttonDriver.update();
      delay(10);
    }
  }
  delay(3000);

  uint8_t attempts = 0;
  uint32_t reactionTime = 0;
  bool success = false;

  do {
    attempts++;

    uint8_t steps = puzzle.getCurrentSteps();
    const uint8_t* seq = puzzle.generateSequence();

    Serial.printf("Attempt #%u: showing %u-step pattern\n", attempts, steps);

    // Display the sequence
    for (uint8_t i = 0; i < steps; ++i) {
      bool pat[4] = {false};
      pat[seq[i]] = true;
      ledDriver.setPattern(pat);
      delay(puzzle.getBlinkInterval());
      ledDriver.clear();
      delay(200);
    }

    // Capture user input
    waitingForInput = true;
    inputIndex = 0;
    unsigned long startMs = millis();
    while (inputIndex < steps) {
      buttonDriver.update();
      delay(10);
    }
    reactionTime = millis() - startMs;
    waitingForInput = false;

    // Compare against the current seq
    success = true;
    for (uint8_t i = 0; i < steps; ++i) {
      if (userSequence[i] != seq[i]) {
        success = false;
        break;
      }
    }

    if (!success) {
      Serial.println("Wrong pattern — generating a new one!");
      buzzerDriver.notify(300, 200, 500);
      delay(500);
    }
  } while (!success);

  // 3) Success
  Serial.printf("Correct in %u attempts, %lums reaction\n",
                attempts, reactionTime);
  buzzerDriver.notify(1000, 200, 500);

  // 4) Record & send metrics
  puzzle.recordPerformance(attempts, reactionTime);
  String response;
  String payload = String("{\"timestamp\":\"") + timeManager.getFormattedTime() +
                   String("\",\"attempts\":") + attempts +
                   String(",\"reaction_time\":") + reactionTime +
                   String("}");
  int status = wifi.httpPost(server, 5000, metricsPath, payload.c_str(), response);
  if (status > 0 && status < 300) {
    Serial.printf("Metrics POST ok: %d\n", status);
  } else {
    Serial.printf("Metrics POST failed: %d\n", status);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize modules
    wifi.begin(30000);
  ledDriver.begin();
  buzzerDriver.begin();
  buttonDriver.begin();
  dhtDriver.begin();
  
  // Initialize time synchronization
  timeManager.begin();
  if (timeManager.sync()) {
    Serial.println("Time synchronized successfully!");
  } else {
    Serial.println("Time synchronization failed!");
  }

  // initialize alarm fetcher
  alarmConfig.begin();
  
  // Register the alarm callback
  alarmScheduler.setCallback(alarmCallback);

}

void loop() {

  // refresh remote alarm periodically
  alarmConfig.update();
  
  // Check if the alarm time has been reached
  alarmScheduler.checkAlarm();
  buttonDriver.update();
  
  // Sensor reading and HTTP sending
  if (millis() - lastPostTime >= interval) {
    lastPostTime = millis();

    if (!dhtDriver.read()) {
      return;

    }

    float temperature = dhtDriver.getTemperature();
    float humidity    = dhtDriver.getHumidity();

    String timestamp = timeManager.getFormattedTime();
    String payload = String("{\"timestamp\":\"") + timestamp +
                     String("\",\"temperature\":") + String(temperature, 2) +
                     String(",\"humidity\":")    + String(humidity, 2) +
                     String("}");

    Serial.printf("Temperature: %.2f C  |  Humidity: %.2f %%\n",
                  dhtDriver.getTemperature(), dhtDriver.getHumidity());
    
    String response;
    int status = wifi.httpPost(
      server,            // domain
      5000,              // port
      sensorPath,        // path
      payload.c_str(),   // JSON body
      response           // response body
    );
 
    if (status > 0) {
      Serial.printf("POST %s -> %d\nResponse: %s\n", sensorPath, status, response.c_str());
    } else {
      Serial.printf("HTTP POST failed, err=%d\n", status);
    }
    
    Serial.println("Next update in (ms): " + String(interval));
    Serial.println();

  }
  
  delay(1000);

}
