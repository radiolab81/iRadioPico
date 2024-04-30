#include "../../player.h"
#include <Arduino.h>
#include "../../globals.h"

#define KEY_CHANNEL_UP_PIN 28
#define KEY_CHANNEL_DOWN_PIN 27
#define KEY_ON_OFF_PIN 22

#define DEBOUNCE_TIME_KEYS 250

enum _result { NONE, CH_UP, CH_DOWN, ON_OFF };

static unsigned long last_interrupt_time = 0;
static _result result = NONE;

static void isr_clockradio_keys() {
  uint32_t flags = save_and_disable_interrupts();
  //hw_clear_bits(&(iobank0_hw->proc0_irq_ctrl.inte[KEY_.... / 8]), 0b1111 << (KEY_.... % 8 )*4u );
   
  //SERIAL.println("in isr");
  if (millis()-last_interrupt_time>DEBOUNCE_TIME_KEYS) {
  
    if (digitalRead(KEY_CHANNEL_UP_PIN)==0){
	  result = CH_UP;
    }
      
    if (digitalRead(KEY_CHANNEL_DOWN_PIN)==0){
      result = CH_DOWN;
    }
    
    if (digitalRead(KEY_ON_OFF_PIN)==0){
      result = ON_OFF;
    }
      
    last_interrupt_time=millis();
  } // if (millis()-last_interrupt_time>DEBOUNCE_TIME_KEYS){
  
  restore_interrupts(flags); 
  //hw_set_bits(&(iobank0_hw->proc0_irq_ctrl.inte[KEY_.... / 8]), 0b0100 << (KEY_.... % 8 )*4u );
}


void gpiod_clockradio_init() {
  // put your setup code here, to run once:
  pinMode(KEY_CHANNEL_UP_PIN,INPUT_PULLUP);
  pinMode(KEY_CHANNEL_DOWN_PIN,INPUT_PULLUP);
  pinMode(KEY_ON_OFF_PIN,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(KEY_CHANNEL_UP_PIN), isr_clockradio_keys, FALLING);
  attachInterrupt(digitalPinToInterrupt(KEY_CHANNEL_DOWN_PIN), isr_clockradio_keys, FALLING);
  attachInterrupt(digitalPinToInterrupt(KEY_ON_OFF_PIN), isr_clockradio_keys, FALLING);
}

void gpiod_clockradio_run() {
  if (result == CH_UP)
    next_station();
    
  if (result == CH_DOWN)
    prev_station();
    
  if (result == ON_OFF) {
  
    if (radiomode == INTERNETRADIO) {
      radiomode = STANDBY;
      SERIAL_PORT.println("GPIOD_CLOCKRADIO: go to mode -> STBY");
    } else {
      radiomode = INTERNETRADIO;
      SERIAL_PORT.println("GPIOD_CLOCKRADIO: go to mode -> INTERNETRADIO");
    }
    
  } // if (result == ON_OFF) {
  
  result = NONE;
}
