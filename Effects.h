// This file has a bunch of visual effects or modifiers that take
// the current frame buffer / video memory and apply an effect,
// then returning the modified frame buffer.


// All functions and variables from the main file that will be used.
#ifndef EFFECTS_H
#define EFFECTS_H
  void loadFrame(const byte frame[], bool overlay, int modify);
  extern int bright_off;
  extern int bright_dim;
  extern int bright_full;
  extern int videoCache[540];
  extern int FrameCounter;
  extern int FireCounter;
#endif



#define numStars 10
float starArray[(numStars * 3)] = {};  // Stars stored as thruples of X,Y,Bright

void renderStars(float bias_x, float bias_y){

  for(int i = 0; i < numStars; i++){
    // Get stored variables.
    float x = starArray[(i*3) + 0];
    float y = starArray[(i*3) + 1];
    float bright = starArray[(i*3) + 2];

    // Update variables.
    x += (x-14)*0.1; 
    y += (y-9)*0.1; 
    bright += bright_dim / 2;
    if ((x == starArray[(i*3) + 0]) && (y == starArray[(i*3) + 1])){
        y = (random(-40 - 70*bias_y,40 + 70*bias_y)/10)+9;
        x = (random(-40 - 100*bias_x,40 + 100*bias_x)/10)+14;
    }
    if (x < 0 || x > 29 || y < 0 || y > 17){
        y = (random(-40 - 70*abs(bias_y),40 + 70*abs(bias_y))/10)+9;
        x = (random(-40 - 100*abs(bias_x),40 + 100*abs(bias_x))/10)+14;
      bright = random(1,2);
    }
    x += bias_x;
    y += bias_y;


    // Blit to screen.
    videoCache[(int(y)*30)+int(x)] = bright;

    // Store variables.
    starArray[(i*3) + 0] = x;
    starArray[(i*3) + 1] = y;
    starArray[(i*3) + 2] = bright;
  }

}

// Visual effect, will cause the lit pixels to randomly dim/brighten.
// Makes lights blinky without actually changing the image.
// int pixels : number of pixels to apply blink to.
void flicker(int pixels){
  for(int p = 0; p < pixels; p++){
    int rng = random(0, 539);
    if(videoCache[rng] == bright_full){
      videoCache[rng] = bright_dim;
    } else if(videoCache[rng] == bright_dim){
      videoCache[rng] = bright_full;
    }
  }
}


// Visual effect, will cause the lit pixels to slowly dim/brighten.
void new_flicker(int frame){
  float offset = frame % 32;
  float brightness[32] = {0.5, 0.598, 0.691, 0.778, 0.854, 0.916, 0.962, 0.99, 1.0, 0.99, 0.962, 0.916, 0.854, 0.778, 0.691, 0.598, 0.5, 0.402, 0.309, 0.222, 0.146, 0.084, 0.038, 0.01, 0, 0.01, 0.038, 0.084, 0.146, 0.222, 0.309, 0.402};
  for(int px = 0; px < 539; px++){
    if(videoCache[px] == bright_full){
      videoCache[px] = int(float(bright_full) * brightness[int(offset) % 32]) + 1;
    }
    offset = (offset + 0.2);
  }
}


// Visual effect, will cause the lit pixels to randomly dim/brighten.
// Makes lights blinky without actually changing the image.
// int pixels : number of pixels to apply blink to.
void invert(){
  for(int p = 0; p < 539; p++){
    if(videoCache[p] == bright_full){
      videoCache[p] = bright_off;
    } else if(videoCache[p] == bright_off){
      videoCache[p] = bright_full;
    }
  }
}


// Overlay glowing outline if fire button pressed.
void fireEffect(){    
  if(digitalRead(6) == 0){
  FireCounter = 0;
  } else if (FireCounter > -bright_full) {
    FireCounter -= bright_full / 10;
  }
  if(FrameCounter % 2 == 0){
    loadFrame(framedither1, true, FireCounter);
  } else {
    loadFrame(framedither2, true, FireCounter);
  }
}


// Shift frame buffer left and right.
// If wrap_around is false, will not display shifted image on other side of screen.
void shift(int shift_x, int shift_y, bool wrap_around){
  // VideoCache is 540  ints long. Stored as 30x18.
  // Create a temporary video cache to modify.
  int videoCacheTemp[540] = {};

  // This does not work, it halts in the first iteration of the loop
  for(int y = 0; y < 18; y++){
    for(int x = 0; x < 30; x++){      
      int new_y = y + shift_y;
      int new_x = x + shift_x;
      if (wrap_around) {
        videoCacheTemp[((30*(new_y % 18))) + (new_x % 30)] = videoCache[(30*y)+x];
      } else {
        if(new_y >= 0 && new_y < 18 && new_x >= 0 && new_x < 30){
          videoCacheTemp[((30*(new_y % 18))) + (new_x % 30)] = videoCache[(30*y)+x];
        }
      }
    }
  }

  // Copy modified video cache to the real video cache.
  for (int a = 0; a < 540; a++){
    videoCache[a] = videoCacheTemp[a];
  }
}



