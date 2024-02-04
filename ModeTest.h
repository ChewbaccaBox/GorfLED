// Mode Test / Program Test
//
//

// All functions and variables from the main file that will be used.
#ifndef MODETEST_H
#define MODETEST_H
void loadFrame(const byte frame[], bool overlay, int modify);
extern int bright_off;
extern int bright_dim;
extern int bright_full;
extern int videoCache[];
extern int FrameCounter;
extern int curr_trig;
extern int curr_lamp;
extern int in_game;
#endif


void modetest() {
  if (FrameCounter < 20) {
    loadFrame(framerouttest, false, 0);
  } else {
    loadFrame(frametestmode, false, 0);
    if (FrameCounter % 50 < 10) {
      loadFrame(framefull, false, 0);
      videoCache[331] = bright_full;
      videoCache[301] = bright_full;
    }

    if (curr_trig != FrameCounter) {
      videoCache[234] = bright_full;  // F
    }
    if (curr_lamp != FrameCounter) {
      videoCache[229] = bright_full;  // L
    }
    if (in_game != 0) {
      videoCache[224] = bright_full;  // G
    }

    //videoCache[219] = bright_full; // Unused 1
    //videoCache[214] = bright_full; // Unused 1
  }
}
