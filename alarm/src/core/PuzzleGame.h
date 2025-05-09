#ifndef PUZZLEGAME_H
#define PUZZLEGAME_H

#include <Arduino.h>

/**
 * PuzzleGame: an adaptive LED/button puzzle.
 *
 * - Call generateSequence() to get a new random pattern of length getCurrentSteps().
 * - Display it using getBlinkInterval() for timing.
 * - After the user finishes, call recordPerformance(attempts, reactionTimeMs).
 *   The module will adapt both sequence length and blink interval over time.
 */
class PuzzleGame {
public:
  struct Performance {
    uint8_t   attempts;
    uint32_t  reactionTime;
  };

  /**
   * @param numLEDs       number of LEDs/buttons available
   * @param baseSteps     initial puzzle length
   * @param historySize   how many past results to keep for adaptation
   * @param blinkMs       initial blink interval in ms
   */
  PuzzleGame(uint8_t numLEDs,
             uint8_t baseSteps     = 4,
             uint8_t historySize   = 5,
             uint32_t blinkMs      = 1000);
  ~PuzzleGame();

  /**
   * Generate a new random sequence of length getCurrentSteps(),
   * store it internally, and return a pointer to it.
   */
  const uint8_t* generateSequence();

  /**
   * Record the user's performance:
   *  - attempts: number of retries needed
   *  - reactionTimeMs: ms between display end and last button press
   * This will adapt the next round's difficulty automatically.
   */
  void recordPerformance(uint8_t attempts, uint32_t reactionTimeMs);

  /** How many steps (LEDs) in the next sequence. */
  uint8_t  getCurrentSteps()   const { return _currentSteps; }

  /** How long (ms) to wait between each LED blink. */
  uint32_t getBlinkInterval()  const { return _blinkInterval; }

  /** Override blink interval manually. */
  void     setBlinkInterval(uint32_t ms) { _blinkInterval = ms; }

private:
  uint8_t   _numLEDs;
  uint8_t   _baseSteps;
  uint8_t   _currentSteps;

  // Ring‐buffer of past performances
  uint8_t       _historySize;
  Performance*  _history;
  uint8_t       _histCount;
  uint8_t       _histIndex;

  uint32_t      _blinkInterval;

  // Helpers
  float   _avgAttempts()     const;
  float   _avgReactionTime() const;
  void    _adaptDifficulty();
};

#endif // PUZZLEGAME_H
