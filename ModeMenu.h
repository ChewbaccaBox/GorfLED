// Power-on Mode Select Menu
// 
// 

// All functions and variables from the main file that will be used.
#ifndef MODEMENU_H
#define MODEMENU_H
  void loadFrame(const byte frame[], bool overlay, int modify);
  extern int bright_off;
  extern int bright_dim;
  extern int bright_full;
  extern int videoCache[];
  extern int FrameCounter;
  extern int mode;
#endif


void modemenu() {
  switch(mode){   // Execute according mode function depending on mode.
    case 0:
      loadFrame(framerout1, false, 0);
      break;
    case 1:
      loadFrame(framerout2, false, 0);  
      break;
    case 2:
      loadFrame(framerout3, false, 0);  
      break;
    case 3:
      loadFrame(framerout4, false, 0);
      break;
  }
}