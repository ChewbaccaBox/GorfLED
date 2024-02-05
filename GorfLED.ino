#include<Wire.h>
#include<avr/pgmspace.h>

#include "Frames.h"
#include "Effects.h"
#include <FlashStorage_SAMD.h> 
#include "Mode1.h"
#include "Mode2.h"
#include "Mode3.h"
#include "Mode4.h"
#include "ModeTest.h"
#include "ModeMenu.h"

const PROGMEM byte PWM_Gama64[64]=
{
  0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,
  0x10,0x12,0x14,0x16,0x18,0x1A,0x1C,0x1E,
  0x20,0x22,0x24,0x26,0x28,0x2A,0x2C,0x2E,
  0x34,0x38,0x3c,0x40,0x44,0x48,0x4b,0x4f,
  0x55,0x5a,0x5f,0x64,0x69,0x6d,0x72,0x77,
  0x7d,0x80,0x88,0x8d,0x94,0x9a,0xa0,0xa7,
  0xac,0xb0,0xb9,0xbf,0xc6,0xcb,0xcf,0xd6,
  0xe1,0xe9,0xed,0xf1,0xf6,0xfa,0xfe,0xff
};

// I2C Addresses for each quadrant's LED Driver
int addresses[4] = {0x68, 0x6A, 0x6C, 0x6E};

// Video RAM
int videoCache[540] = {};

// Initial values are overwritten by loop using potentiometer
int bright_full = 20;  // Relative brightess. Set by pot.
int bright_dim = 6;   // Relative dim brightness. Set by pot.
int bright_off = 0;   // Absolute off brightess.

// For the "mode" button
int curr_button = 1;      // Current state of button.   // 0 is on, 1 is off.
int prev_button = 1;      // Previous state of button.
int mode = 4;             // Current mode, or which routine is being preformed currently. Starts in test mode.
int prev_mode = 0;        // Ensures EEPROM isn't overwritten each time the display turns on.
int power_on_mode = 0;    // Power on mode select, 0 = disabled. Is decided in setup().
int first_depress = 0;    // Waits until first depress of fire button if initially pressed for inputs.

// For the rest of the I/O (Lamp and Trigger inputs)    // 0 is on, 1 is off.
int curr_lamp = -1;      // Current state of lamp.      
int prev_lamp = -1;      // Previous state of lamp.
int curr_trig = -1;      // Current state of trigger.
int prev_trig = -1;      // Previous state of trigger.
int in_game = 0;        // Whether or not the display thinks the machine is in a game or not.

// For animations
int FrameCounter = 0;   // Decides what is done in a routine on a given frame. Continously increments.
int FireCounter = -bright_full; // Frames since fire button pressed.
//int faux_pot_value = 0; // Fake pot value for testing, repeatedly cycles from 1024 to 0

#define num_modes 5;

// For saving mode after power off and on
FlashStorage(mode_storage, int);

void setup() {
  // Initialize communications with LED driver chips
  Wire.begin();
  Wire.setClock(400000);        // Set up I2C @ 400KHz
  init3729();  // Intialize chips.
  
  // Set up I/O to microcontroller
  pinMode(4, INPUT);  // Button on back of board - has physical pull up.
  pinMode(6, INPUT_PULLDOWN);  // "Lamp" input pad - enable internal pulldown
  pinMode(7, INPUT_PULLDOWN);  // "Trigger" input pad - enable internal pulldown

  // Debug
  Serial.begin(9600); // open the serial port at 9600 bps (for debugging)

  // Read last saved mode from memory.
  mode_storage.read(mode);
  prev_mode = mode;

  // Power-on mode menu
  if(digitalRead(7) == 0){  // If trigger is pressed, enter power-on menu.
    power_on_mode = 1;
    first_depress = 1;
  }

}


void loop() {
  // Poll for inputs.
  bright_full = max(analogRead(3) / 40, 2);   // Set relative 'full' brightness depending on pot / 40
  bright_dim = max(analogRead(3) / 100, 1);   // Set relative 'dim' brightness depending on pot / 100

  // Digital inputs - lamp, trigger, and onboard button.
  // Their values are set to the last frame they were inactive on.
  // This way you can tell the difference between a long and a short press.
  if (digitalRead(4)) { curr_button = FrameCounter; }
  if (digitalRead(6)) { curr_lamp = FrameCounter; }
  if (digitalRead(7)) { curr_trig = FrameCounter; }

  // Draw next frame.
  switch(mode){   // Execute according mode function depending on mode.
      case 0:
        mode1();  
        break;
      case 1:
        mode2();  
        break;
      case 2:
        mode3();  
        break;
      case 3:
        mode4();  
        break;
      case 4:
        modetest();  
        break;
  }

  // Mode selection via back button
  if ((curr_button == FrameCounter) && (FrameCounter - prev_button) > 2){   // On a short button press, cycle "mode" 0 -> 3
                                                       // Also, reset the framecounter so routine starts from beginning.
    if (FrameCounter < 20){
      mode  = (mode + 1) % num_modes;
    }
    FrameCounter = 0;

  }     


  if ((FrameCounter == 100) && !power_on_mode && (prev_mode != mode)){    // If the user has left the mode selected for a while, flash it into memory.
    mode_storage.write(mode);                      // 100 frames is about 5 seconds or so.
    //loadFrame(frameroutsave, false, 0);
    prev_mode = mode;
    //Serial.println("Flashed mode to memory.");
  }


  // Power On Mode Select Menu
  if(power_on_mode){    // Enabled if fire button is pressed during startup.                
    modemenu();
    if(prev_trig != -1){
      if((FrameCounter - prev_trig) > 16 ){           // If long press, save mode              (curr = fc means button now inactive. fc - prev means was just held for >16 frames)
        mode_storage.write(mode);                                                   // and disable power-on menu
        power_on_mode = 0;
        prev_mode = mode;
        loadFrame(frameroutsave, false, 0);
      } else if ((((curr_trig == FrameCounter) && (FrameCounter - prev_trig) > 2))) {   // If short press, cycle mode.
        mode  = (mode + 1) % num_modes;   
        FrameCounter = 0;                                                        
      }
    }
  }

  // Update previous inputs. Used for checking for changes.
  prev_button = curr_button;
  prev_lamp = curr_lamp;
  prev_trig = curr_trig;

  // Lamp is on every time a shot is barreled. 
  // If lamp on, set in_game to 40. Otherwise count it down. Consider not in game if value is 0.
  if(FrameCounter != curr_lamp && FrameCounter > 5){
    in_game = 100;
  } else if (in_game > 0){
    in_game--;
  }

  FrameCounter++;  // Increment FrameCounter.
  //Serial.println(FrameCounter);
  //faux_pot_value = (faux_pot_value + 40) % 1024;

  // Commit everything to the screen.
  updateDisplay();                    
}


// Transmit data to the LED Drivers over I2C
void IS_IIC_WriteByte(uint8_t Dev_Add,uint8_t Reg_Add,uint8_t Reg_Dat) {
  Wire.beginTransmission(Dev_Add/2);  // Open transmissions to IS31FL3729
  Wire.write(Reg_Add);                // Send register address
  Wire.write(Reg_Dat);                // Send register data
  Wire.endTransmission();             // Stop transmitting
}


// Initialize the LED Drivers.
void init3729() {
  for(int i = 0; i < 4; i++){                   // For each LED driver
    for(uint8_t j=90; j<0x9f; j+=1)             // For each current sink (15 rows of pixels)
    {
      IS_IIC_WriteByte(addresses[i],j,0x50);    // Set Scaling Registers - decides peak current of each LED
    }
    IS_IIC_WriteByte(addresses[i],0xA0,0x01);   // Set Configuration Register - 9SWx15CS Matrix with normal operation, no short detection, 1.4v LED
    IS_IIC_WriteByte(addresses[i],0xA1,0x7F);   // Set Global Current Control Register
  }
}


// Refresh the buffer of each LED Driver with the contents of VideoCache
void updateDisplay(){
  int i = 0; // Index in the Video Cache as we update the screen.
  for (int x = 0x00; x <= 0x80; x += 0x10) {
    for (int y = 0x01; y <= 0x0F; y += 0x01) {
      int coeff = ((i % 15) + ((i / 15) * 30));
      IS_IIC_WriteByte(addresses[1], (x + y), pgm_read_byte_near(&PWM_Gama64[videoCache[coeff]]));        // Simultaneously set each quadrant
      IS_IIC_WriteByte(addresses[0], (x + y), pgm_read_byte_near(&PWM_Gama64[videoCache[coeff + 15]]));   // at the x/y value instead of having
      IS_IIC_WriteByte(addresses[3], (x + y), pgm_read_byte_near(&PWM_Gama64[videoCache[coeff + 270]]));  // a loop for each.
      IS_IIC_WriteByte(addresses[2], (x + y), pgm_read_byte_near(&PWM_Gama64[videoCache[coeff + 285]]));
      i++;
    }
  }
}


// Decode byte array from Frames.h and paste onto VideoCache
// frame[] - the actual byte array reference
// overlay - false = rewrite all of screen, true = overlay on top of existing VideoCache
// modify  - number of brightness levels to step up / down by. does not affect off pixels.
void loadFrame(const byte frame[], bool overlay, int modify){
  int arraySize = frame[0];
  int vMemIndex = 3;

  for(int k = 1 ; k < arraySize + 1 ; k++) {

    byte currentByte = frame[k];
    if((bitRead(currentByte, 7) == 1) && (bitRead(currentByte, 6) == 1)) { // If the two MSB bits are 11, then this byte is a compressed byte.
      int numSkips = currentByte - 192;  // The 4 LSB bits are the # of groups of 4 black pixels to skip. Each pixel is 2 bits.

      for(int b = 0 ; b < (numSkips * 4) ; b += 1) {
        if(!overlay){
          videoCache[vMemIndex] = bright_off;
        }
        vMemIndex = cornerCutIncrement(vMemIndex, !overlay);
        
      }  
    } else { 
      for(int b = 7 ; b >= 0 ; b -= 2) {  // Otherwise, it's just 4 pixels, each with 2 bits.
        if(bitRead(currentByte, b) == 1 && bitRead(currentByte, b - 1) == 0){         // Current pixel is "10" : Red
          videoCache[vMemIndex] = max(bright_full + modify, 1);
        } else if (bitRead(currentByte, b) == 0 && bitRead(currentByte, b - 1) == 1){ // Current pixel is "01" : Dark Red
          videoCache[vMemIndex] = max(bright_dim + modify, 0);
        } else if (bitRead(currentByte, b) == 0 && bitRead(currentByte, b - 1) == 0){ // Current pixel is "00" : Black
          if(!overlay){
            videoCache[vMemIndex] = bright_off;
          }
        }
        vMemIndex = cornerCutIncrement(vMemIndex, !overlay);
      }
    }
  }

  if (vMemIndex < 540){
    while(vMemIndex < 540){
      if(!overlay){
        videoCache[vMemIndex] = bright_off;
      }
      vMemIndex = cornerCutIncrement(vMemIndex, !overlay);
    }
  }

}


// Helper function to loadFrame. Used for skipping corners of display where
// no pixels physically exist, to align with how frames are stored.
// Overwrite, when true, will set all the invisible corner pixels to black.
int cornerCutIncrement(int index, bool overwrite){
    int nextIndex = index + 1;
    if(nextIndex == 506)                                                 { nextIndex += 8; }
    if(nextIndex == 477)                                                 { nextIndex += 7; }
    if(nextIndex == 447)                                                 { nextIndex += 6; }
    if(nextIndex == 27 || nextIndex == 418)                                  { nextIndex += 5; }
    if(nextIndex == 58 || nextIndex == 388)                                  { nextIndex += 4; }
    if(nextIndex == 88 || nextIndex == 359)                                  { nextIndex += 3; }
    if(nextIndex == 119 || nextIndex == 149 || index == 299 || index == 329) { nextIndex += 2; }
    if(nextIndex == 179 || nextIndex == 270)                                 { nextIndex += 1; }

    if(overwrite){
      for (int i = index + 1; i < nextIndex; i++){
        videoCache[i] = 0;
      }
    }

    return nextIndex;
}