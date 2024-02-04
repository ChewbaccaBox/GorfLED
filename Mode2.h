// Mode 2 / Program 2
// 
// 

// All functions and variables from the main file that will be used.
#ifndef MODE2_H
#define MODE2_H
  void loadFrame(const byte frame[], bool overlay, int modify);
  extern int bright_off;
  extern int bright_dim;
  extern int bright_full;
  extern int videoCache[];
  extern int FrameCounter;
  extern float starArray[];
  void renderStars();
  extern int in_game;
#endif

float traj_x;
float traj_y;
float pos_x;
float pos_y;
int next_encounter;
int last_encounter;
float accel;
int type;
float bias_x;
float bias_y;

void randomize_encounter(){
    if(FrameCounter > next_encounter){  // If it's time for a new encounter,
    last_encounter = next_encounter;
    next_encounter = FrameCounter + random(70,210); // Set a random time for the next one.
    traj_x = random(-20,20) / float(10);     // randomize trajectory velocity for X
    traj_y = random(-20,20) / float(10);     // randomize trajectory velocity for Y
    pos_x = traj_x * -30;             // X - start off screen opposite of traj. direction.
    pos_y = traj_y * -30;             // Y - start off screen opposite of traj. direction.
    type = random(0,5);               // Randomize sprite art.
    accel = 1;
    if (traj_x < 0.7 && traj_y < 0.7){    // If trajectory velocity ends up being 0, redo.
      next_encounter = FrameCounter;  // Redo it next frame.
      pos_y = 50;                     // Put it off screen.
    }
  }
}

void mode2() {
  if (FrameCounter < 20){
    loadFrame(framerout2, false, 0);
    
    for(int i = 0; i > numStars; i++){
        int y = random(-5,5)+9;
        int x = random(-10,10)+14;
        int bright = random(0,5);
                // Store variables.
        starArray[(i*3) + 0] = x;
        starArray[(i*3) + 1] = y;
        starArray[(i*3) + 2] = bright;
    }

    randomize_encounter();

  } else {
    if(in_game == 0){
      randomize_encounter();
    }
    if (pos_x < 60 && pos_x > -60 && pos_y > -60 && pos_y < 60){
      pos_x += traj_x;
      pos_y += traj_y;
      traj_x = traj_x * accel;
      traj_y = traj_y * accel;
    }
    loadFrame(frameblank, false, 0);
    switch(type){
      case 0:
        loadFrame(framespinvader, true, 0);
        break;
      case 1:
        loadFrame(framespinvUFO, true, 0);
        break;
      case 2:
        loadFrame(framefleet, true, 0);
        break;
      case 3:
        loadFrame(frameasteroid, true, 0);
        break;
      case 4:
        loadFrame(framemothership, true, 0);
        break;
    }
    shift(int(pos_x), int(pos_y), false);
    renderStars(bias_x * 0.8, bias_y * 0.8);
    
    bias_x = (bias_x + (traj_x / 5)) * 0.65;
    bias_y = (bias_y + (traj_y / 5)) * 0.65;
    
  }
}

