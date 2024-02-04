// Mode 4 / Program 4
// 
// 

// All functions and variables from the main file that will be used.
#ifndef MODE4_H
#define MODE4_H
  void loadFrame(const byte frame[], bool overlay, int modify);
  extern int bright_off;
  extern int bright_dim;
  extern int bright_full;
  extern int videoCache[540];
  extern int FrameCounter;
  extern int prev_trig;
  extern int curr_trig;
  extern int in_game;
  extern float starArray[];
  void renderStars();
#endif

int ShotCounter = 25;

void mode4() {
  if (FrameCounter < 20){
    loadFrame(framerout4, false, 0);
    
    for(int i = 0; i > numStars; i++){
        int y = random(-5,5)+9;
        int x = random(-10,10)+14;
        int bright = random(0,5);
                // Store variables.
        starArray[(i*3) + 0] = x;
        starArray[(i*3) + 1] = y;
        starArray[(i*3) + 2] = bright;
    }
  } else {
    loadFrame(frameguns, false, -3);
    renderStars(0, 0);

    if(in_game != 0){
      if ((FrameCounter - prev_trig) == 2) {
        if (ShotCounter < 6) {
          ShotCounter = 0;
        } else {
          ShotCounter = -2;
        }
      }
    }

    if((ShotCounter < 4) && (ShotCounter > 0)){
      loadFrame(frameshoottip, true, (bright_full + random(-20,10)));
    }
    if((((ShotCounter + 1) % 50) < 4) && ((ShotCounter + 1) > 0)){
      loadFrame(frameshootmid, true, (bright_full + random(-20,10)));
    }
    if((((ShotCounter + 2) % 50) < 4) && ((ShotCounter + 2) > 0)){
      loadFrame(frameshootbase, true, (bright_full + random(-20,10)));
    }
    
    if(ShotCounter < 30){
      ShotCounter++;
    }
  }
}