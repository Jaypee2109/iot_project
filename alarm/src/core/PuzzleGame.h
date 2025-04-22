#ifndef PUZZLEGAME_H
#define PUZZLEGAME_H

#include <Arduino.h>

class PuzzleGame {
  public:
    /**
     * Constructs a PuzzleGame for a given number of LEDs.
     *
     * @param numLEDs Total number of LEDs used in the puzzle.
     */
    PuzzleGame(uint8_t numLEDs);

    /**
     * Generates a random sequence of LED indices.
     *
     * @param steps    The number of steps (i.e. random selections) to generate.
     * @param sequence A pre-allocated array of length 'steps' where the random LED indices (0 to numLEDs-1) will be stored.
     */
    void generateRandomSequence(uint8_t steps, uint8_t* sequence);

    // Optionally, add functions to validate user input, etc.
    
  private:
    uint8_t _numLEDs;
};

#endif
