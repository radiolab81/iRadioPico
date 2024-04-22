#include <Arduino.h>
#include "globals.h"
#include "player.h"

#define ANALOG_PIN A2

#define DELAY_TIME 100

#define LOWEST_VALUE 0
#define HIGHEST_VALUE 1024

#define STATION_GAPS (HIGHEST_VALUE-LOWEST_VALUE)/channels_in_list

static unsigned long last_call_time = 0;

int getChannelNoFromADC(int adc_val) {
    for (int i=0; i<channels_in_list; i++) {
         if ( (adc_val >= (LOWEST_VALUE + i*STATION_GAPS) ) &&
              (adc_val <= (LOWEST_VALUE + i*STATION_GAPS)+STATION_GAPS)) {
           return i; 
         }
    }
    return 0;
}


void gpiod_potentiometer_init() {
  analogReadResolution(10 /*int bits*/);
}

void gpiod_potentiometer_run() { 

 if (millis()-last_call_time > DELAY_TIME) {
    int adc_val = analogRead(ANALOG_PIN);
    
    if (actual_channel_or_file_ID != getChannelNoFromADC(adc_val) ) {
      goto_station(getChannelNoFromADC(adc_val));
    }
    
    //SERIAL_PORT.print("ADC value is = "); SERIAL_PORT.println(analogRead(ANALOG_PIN));
    //SERIAL_PORT.print("Channel value is = "); SERIAL_PORT.println(getChannelNoFromADC(analogRead(ANALOG_PIN)));
 
    last_call_time=millis();
  }  // if (millis()-last_interrupt_time>DELAY_TIME) 
}
