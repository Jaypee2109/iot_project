#include "PuzzleGame.h"

PuzzleGame::PuzzleGame(uint8_t numLEDs) : _numLEDs(numLEDs) {
  // Nothing else needed in the constructor.
}

void PuzzleGame::generateRandomSequence(uint8_t steps, uint8_t* sequence) {
  for (uint8_t i = 0; i < steps; i++) {
    // Generate a random LED index between 0 and _numLEDs-1
    sequence[i] = random(0, _numLEDs);
  }
}
