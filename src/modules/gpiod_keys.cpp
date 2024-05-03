#include "player.h"
#include <Arduino.h>

#define KEY_CHANNEL_UP_PIN 28
#define KEY_CHANNEL_DOWN_PIN 27

#define DEBOUNCE_TIME_KEYS 250

enum _result { NONE, CH_UP, CH_DOWN };

static unsigned long last_interrupt_time = 0;
static _result result = NONE;

void handle_keys() {
  if ( (digitalRead(KEY_CHANNEL_UP_PIN)==0) || (digitalRead(KEY_CHANNEL_DOWN_PIN)==0) )
  if (millis()-last_interrupt_time>DEBOUNCE_TIME_KEYS) {
  
    if (digitalRead(KEY_CHANNEL_UP_PIN)==0){
	  result = CH_UP;
    }
      
    if (digitalRead(KEY_CHANNEL_DOWN_PIN)==0){
      result = CH_DOWN;
    }

    last_interrupt_time=millis();
  } // if (millis()-last_interrupt_time>DEBOUNCE_TIME_KEYS){
}


void gpiod_keys_init() {
  // put your setup code here, to run once:
  pinMode(KEY_CHANNEL_UP_PIN,INPUT_PULLUP);
  pinMode(KEY_CHANNEL_DOWN_PIN,INPUT_PULLUP);
}

void gpiod_keys_run() {
  handle_keys();
  if (result == CH_UP)
    next_station();
    
  if (result == CH_DOWN)
    prev_station();
    
  result = NONE;
}
