#include <Arduino.h>
#include "globals.h"
#include "lfs.h"

#define INTERVAL_SAVE_MS_AFTER_CHANGE 10000


static unsigned long last_auto_save_check_time = 0;
static unsigned int last_actual_channel_or_file_ID = 0;

void auto_save_init() {

}

void auto_save_run() {

   if (millis()-last_auto_save_check_time > INTERVAL_SAVE_MS_AFTER_CHANGE) {
     // check if somethk changed since last run
     if (actual_channel_or_file_ID != last_actual_channel_or_file_ID) {
       writeRadioSettingsLittleFS();
     }  
       
   
     last_actual_channel_or_file_ID = actual_channel_or_file_ID;  
     last_auto_save_check_time=millis();
   }
}
