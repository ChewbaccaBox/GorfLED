// Mode 1 / Program 1
// Gomez Glimmer
// The original Gomez pattern but blinky in a slow, fading way.

// All functions and variables from the main file that will be used.
#ifndef MODE1_H
#define MODE1_H
  void loadFrame(const byte frame[], bool overlay, int modify);
  extern int bright_off;
  extern int bright_dim;
  extern int bright_full;
  extern int videoCache[];
  extern int FrameCounter;
#endif

int level = 0;

void mode1() {
  if (FrameCounter < 20){
    loadFrame(framerout1, false, 0);
  } else {
    loadFrame(frameGomez, false, 0);
    float offset = FrameCounter % 32;
    float brightness[32] = {0.5, 0.598, 0.691, 0.778, 0.854, 0.916, 0.962, 0.99, 1.0, 0.99, 0.962, 0.916, 0.854, 0.778, 0.691, 0.598, 0.5, 0.402, 0.309, 0.222, 0.146, 0.084, 0.038, 0.01, 0, 0.01, 0.038, 0.084, 0.146, 0.222, 0.309, 0.402};
    for(int px = 0; px < 539; px++){
      if(videoCache[px] == bright_full){
        videoCache[px] = int(float(bright_full) * brightness[int(offset) % 32]) + 1;
      }
      offset = (offset + 7.8);
    }
  }
}