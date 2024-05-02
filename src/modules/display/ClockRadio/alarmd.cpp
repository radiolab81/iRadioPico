#include <Arduino.h>
#include "hardware/rtc.h"
#include "../../globals.h"
#include "../../lfs.h"

 
static unsigned long last_call_alarmd_run_time = 0;
#define TASK_ARARLM_DELAY 30000
 
static datetime_t time_now;

void alarmd_init() {
  SERIAL_PORT.println("ALARMD: init ...read radio-settings...");
  #ifdef USE_LITTLEFS
     #ifdef USE_AUTO_SAVE
        readRadioSettingsLittleFS();  // actual_channel_or_file_ID, volume_L/R ... overwritten by settings.txt 
     #endif
  #endif
}


void alarmd_run() {

  if (millis()-last_call_alarmd_run_time > TASK_ARARLM_DELAY) {
    if (rtc_running) {
      SERIAL_PORT.println("ALARMD: RTC is running ...");
      if (alarm1_status == ALARM_ENABLED) {
        SERIAL_PORT.println("ALARMD: ALARM1 is enabled, check for wakeup ...");
        rtc_get_datetime(&time_now);
        if ( (time_now.hour == alarm1.hour) &&
             (time_now.min == alarm1.min) ) {
             SERIAL_PORT.println("ALARMD: ring ring ... wake up");
             radiomode = INTERNETRADIO; // other wake up stuff do in mode_switcher.cpp
             // do here stuff like set wakeup channel, wakeup volume etc.
             // ...
           }
      } // if (alarm1_status == ALARM_ENABLED) { 
      else {
        SERIAL_PORT.println("ALARMD: is running but ALARM1 is disabled ...");
      }
    } // if (rtc_running) {
    last_call_alarmd_run_time = millis();
  }
  
}

