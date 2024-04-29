#include <Arduino.h>
#include "globals.h"
#include "lfs.h"

#define INTERVAL_LOOKING_FOR_CHANGE_IN_RADIO_SETTINGS 10000


static unsigned long last_check_time = 0;
static unsigned int  last_actual_channel_or_file_ID = 0;

void auto_save_init() {

}

void auto_save_run() {

   if (millis()-last_check_time > INTERVAL_LOOKING_FOR_CHANGE_IN_RADIO_SETTINGS) {
     // check if somethk changed since last run
     if (actual_channel_or_file_ID != last_actual_channel_or_file_ID) {
       writeRadioSettingsLittleFS();
     }  
       
   
     last_actual_channel_or_file_ID = actual_channel_or_file_ID;  
     last_check_time=millis();
   }
}
