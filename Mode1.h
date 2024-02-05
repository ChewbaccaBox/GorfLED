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
  extern int prev_lamp;
  extern int curr_lamp;
  extern int in_game;
#endif

float level;  // Current brightness modifier.

void mode1() {
  if (FrameCounter < 20){
    loadFrame(framerout1, false, 0);
    level = bright_full;
  } else {
    loadFrame(frameGomez, false, 0);

    // If in game, vary level. Otherwise keep level @ 0.
    // Lamp on = bright. Increase to bright_full 
    // Lamp off = dim. Decrease to bright_dim.
    if(in_game == 0){ // If not in game, then
      if(level < bright_full){ level++; } // Increase brightness to full 
    } else {  // Otherwise,
      for(int i = 0; i < 4; i++){ // Double time!
        if(curr_lamp == prev_lamp && level < bright_full){ level++; } // If light is on, make it brighter.
        if(curr_lamp != prev_lamp && level > 0){ level--; } // If light is off, make it dimmer.
      }
      level = max(level, 0); // Enact boundaries in case of brightness being adjusted while game is on
      level = min(level, bright_full); 
    }



    float offset = FrameCounter % 32;
    float brightness[32] = {0.5, 0.598, 0.691, 0.778, 0.854, 0.916, 0.962, 0.99, 1.0, 0.99, 0.962, 0.916, 0.854, 0.778, 0.691, 0.598, 0.5, 0.402, 0.309, 0.222, 0.146, 0.084, 0.038, 0.01, 0, 0.01, 0.038, 0.084, 0.146, 0.222, 0.309, 0.402};
    for(int px = 0; px < 539; px++){
      if(videoCache[px] == bright_full){
        videoCache[px] = int(float(level) * brightness[int(offset) % 32]) + 1;
      }
      offset = (offset + 7.8);
    }
  }
}