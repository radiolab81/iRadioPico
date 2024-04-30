#include <Arduino.h>

#include "../../globals.h"
#include "../../player.h"


static unsigned long last_call_time = 0;
#define TASK_DELAY 1000

static RadioMode  last_radiomode = INTERNETRADIO;

void mode_switcher_init() {

}


void mode_switcher_run() {
  if (millis()-last_call_time > TASK_DELAY) {
  
    if (last_radiomode != radiomode) {
    
      // change to internetradio
      if (radiomode == INTERNETRADIO) {
        start_player();
        
        // ... more to do ? 
      }
      
      // go into standby
      if (radiomode == STANDBY) {
        stop_player();
        
        // ... more to do? 
      }
      
      //....
    
    }
    
    last_radiomode = radiomode;
    last_call_time = millis();
  }
}

