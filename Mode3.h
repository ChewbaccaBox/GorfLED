// Mode 3 / Program 3
// 
// 

// All functions and variables from the main file that will be used.
#ifndef MODE3_H
#define MODE3_H
  void loadFrame(const byte frame[], bool overlay, int modify);
  extern int bright_off;
  extern int bright_dim;
  extern int bright_full;
  extern int videoCache[];
  extern int FrameCounter;
#endif


void mode3() {              // Gomez Slow Scan
  if (FrameCounter < 20){
    loadFrame(framerout3, false, 0);
  } else {
    loadFrame(frameGomez, false, 0);
    float offset = FrameCounter % 32;
    float brightness[32] = {0.5, 0.598, 0.691, 0.778, 0.854, 0.916, 0.962, 0.99, 1.0, 0.99, 0.962, 0.916, 0.854, 0.778, 0.691, 0.598, 0.5, 0.402, 0.309, 0.222, 0.146, 0.084, 0.038, 0.01, 0, 0.01, 0.038, 0.084, 0.146, 0.222, 0.309, 0.402};
    for(int px = 0; px < 539; px++){
      if(videoCache[px] == bright_full){
        videoCache[px] = int(floor(float(bright_full) * brightness[int(offset) % 32])) * 1.5 + 1;
      }
      offset = (offset + 0.2);
    }
  }
}