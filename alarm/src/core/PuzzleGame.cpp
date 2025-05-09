#include "core/PuzzleGame.h"

// Constructor: allocate history buffer and sequence buffer
PuzzleGame::PuzzleGame(uint8_t numLEDs,
                       uint8_t baseSteps,
                       uint8_t historySize,
                       uint32_t blinkMs)
  : _numLEDs(numLEDs)
  , _baseSteps(baseSteps)
  , _currentSteps(baseSteps)
  , _historySize(historySize)
  , _histCount(0)
  , _histIndex(0)
  , _blinkInterval(blinkMs)
{
  // allocate performance history
  _history = new Performance[_historySize];
}

PuzzleGame::~PuzzleGame() {
  delete[] _history;
}

const uint8_t* PuzzleGame::generateSequence() {
  // Fill internal buffer indices [0.._currentSteps-1]
  for (uint8_t i = 0; i < _currentSteps; ++i) {
    _history[i].attempts = 0; // unused here, just to silence warnings
  }
  static uint8_t* sequence = nullptr;
  // allocate the sequence buffer once
  if (!sequence) {
    sequence = new uint8_t[_numLEDs];
  }
  for (uint8_t i = 0; i < _currentSteps; ++i) {
    sequence[i] = random(0, _numLEDs);
  }
  return sequence;
}

void PuzzleGame::recordPerformance(uint8_t attempts, uint32_t reactionTimeMs) {
  // write into ring buffer
  _history[_histIndex] = { attempts, reactionTimeMs };
  _histIndex = (_histIndex + 1) % _historySize;
  if (_histCount < _historySize) _histCount++;

  // adjust for next round
  _adaptDifficulty();
}

float PuzzleGame::_avgAttempts() const {
  if (_histCount == 0) return _baseSteps;
  uint32_t sum = 0;
  for (uint8_t i = 0; i < _histCount; ++i) {
    sum += _history[i].attempts;
  }
  return float(sum) / _histCount;
}

float PuzzleGame::_avgReactionTime() const {
  if (_histCount == 0) return 0.0f;
  uint64_t sum = 0;
  for (uint8_t i = 0; i < _histCount; ++i) {
    sum += _history[i].reactionTime;
  }
  return float(sum) / _histCount;
}

void PuzzleGame::_adaptDifficulty() {
  float avgA = _avgAttempts();
  float avgR = _avgReactionTime() / 1000.0f; // convert ms→s

  Serial.print(avgA);
  Serial.print(avgR);
  

  // if struggling: shorten & slow down
  if (avgA >= 3 && avgR > 5.0f) {
    _currentSteps--;
    _blinkInterval = min(_blinkInterval + 100, 2000U);
  }
  // if excelling: lengthen & speed up
  else if (avgA < 2 || avgR < 3.0f) {
    _currentSteps++;
    _blinkInterval = max(_blinkInterval - 100, 200U);
  }

  Serial.printf(
    "Adapted → steps: %u, blink: %ums (avgA=%.2f, avgR=%.2fs)\n",
    _currentSteps, _blinkInterval, avgA, avgR
  );
}
