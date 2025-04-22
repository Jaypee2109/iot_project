#include <hal/LEDDriver.h>
#include <hal/DHTDriver.h>
#include <hal/BuzzerDriver.h>
#include <hal/ButtonDriver.h>
#include <core/AlarmScheduler.h>
#include <core/PuzzleGame.h>
#include <core/TimeSync.h>
#include <credentials.h>

// --- Global Variables for Alarm Input ---
const uint8_t steps = 4;                // Number of steps in the sequence
uint8_t correctSequence[steps] = {0};     // Holds the randomly generated correct pattern (LED indices)
uint8_t userSequence[steps] = {0};        // Holds the player's input (LED indices)
volatile uint8_t inputIndex = 0;          // Next free index for player input
volatile bool waitingForInput = false;    // Flag set when waiting for player's input
volatile bool alarmCancel = false;        // Flag set when a button cancels the alarm warning phase

// --- Button Callback ---
// This callback is triggered on a release edge (by our ButtonDriver implementation)
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

// ButtonDriver setup (using an initializer list)
ButtonDriver buttonDriver({39, 38, 37, 36}, onButtonPressed);

// DHTDriver setup
const int dht_sda = 21;
const int dht_scl = 22;
const char server[] = "18.188.56.179";
const char path[] = "/sensor";
DHTDriver dhtDriver(WIFI_SSID, WIFI_PASS, server, path);

// TimeSync setup
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long gmtOffset_sec = -28800;    // California Standard Time (-8 hours from UTC)
const int daylightOffset_sec = 3600;    // DST adjustment: +1 hour (effective: -7 hours)
TimeSync timeManager(ntpServer1, ntpServer2, gmtOffset_sec, daylightOffset_sec);

// AlarmScheduler and PuzzleGame modules
AlarmScheduler alarmScheduler;
PuzzleGame puzzleGame(4);   // Using 4 LEDs for the puzzle

unsigned long lastPostTime = 0;
const int interval = 5000;  // 5-second interval between sensor updates

// --- Alarm Callback ---
// This function is invoked when the alarm time is reached
void alarmCallback() {
  Serial.println("Alarm triggered!");

  // 1. Alarm Warning Phase: Keep buzzing until a button (full press–release) is detected
  Serial.println("Warning: Buzzing until a button is pressed.");
  alarmCancel = false;  // Reset the cancellation flag
  const unsigned long warningDuration = 1000; // each beep lasts 1000 ms
  while (!alarmCancel) {
    // Trigger a beep.
    buzzerDriver.notify(500, 200, warningDuration);  // 500 Hz tone, volume 200, duration 1000 ms
    // Within the beep duration, update the button state to allow edge detection
    unsigned long start = millis();
    while ((millis() - start) < warningDuration && !alarmCancel) {
      buttonDriver.update();
      delay(10);
    }
  }
  delay(3000);  // Brief pause after cancellation

  // 2. Generate a Random Sequence for the Puzzle
  puzzleGame.generateRandomSequence(steps, correctSequence);
  Serial.print("Correct sequence (LED indices): ");
  for (uint8_t i = 0; i < steps; i++) {
    Serial.print(correctSequence[i]);
    Serial.print(" ");
  }
  Serial.println();

  // 3. Display the Random Sequence on the LED Display
  for (uint8_t i = 0; i < steps; i++) {
    uint8_t ledIndex = correctSequence[i];
    bool pattern[4] = { false, false, false, false };

    if (ledIndex < 4) {
      pattern[ledIndex] = true;

    }

    Serial.print("Displaying step ");
    Serial.print(i + 1);
    Serial.print(": LED index ");
    Serial.println(ledIndex);
    ledDriver.setPattern(pattern);
    delay(1000);
    ledDriver.clear();
    delay(300);

  }
  
  // 4. Allow the Player to Recreate the Pattern
  Serial.println("Recreate the pattern using the buttons.");
  waitingForInput = true;
  inputIndex = 0;

  // Wait until the player has entered 'steps' button presses
  while (inputIndex < steps) {
    buttonDriver.update();
    delay(10);
  }
  waitingForInput = false;
  
  // Display the player's recorded sequence
  Serial.print("Player sequence: ");
  for (uint8_t i = 0; i < steps; i++) {
    Serial.print(userSequence[i]);
    Serial.print(" ");
  }
  Serial.println();

  // 5. Validate the Player's Input Against the Correct Sequence
  bool success = true;
  for (uint8_t i = 0; i < steps; i++) {
    if (userSequence[i] != correctSequence[i]) {
      success = false;
      break;
    }
  }
  
  
  delay(1000);
  
  if (success) {
    Serial.println("Correct pattern! Alarm ended.");
    for (int i = 0; i < 3; ++i) {
      buzzerDriver.notify(1000, 200, 300);
      delay(300);
   }
   
  } else {
    Serial.println("Incorrect pattern. Restarting alarm.");
    buzzerDriver.notify(300, 200, 2000);
    alarmCallback();  // Restart the alarm callback
  }

  delay(3000);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize modules
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
  
  alarmScheduler.setAlarm(21, 59);
  
  // Register the alarm callback
  alarmScheduler.setCallback(alarmCallback);

}

void loop() {
  
  // Check if the alarm time has been reached
  alarmScheduler.checkAlarm();
  buttonDriver.update();
  
  // Sensor reading and HTTP sending
  if (millis() - lastPostTime >= interval) {
    lastPostTime = millis();

    if (!dhtDriver.readSensor()) {
      return;

    }

    Serial.printf("Temperature: %.2f C  |  Humidity: %.2f %%\n",
                  dhtDriver.getTemperature(), dhtDriver.getHumidity());
    dhtDriver.sendData();
    Serial.println("Next update in (ms): " + String(interval));
    Serial.println();

  }
  
  delay(1000);

}
