#include "player.h"
#include <Arduino.h>

#define KEY_A_PIN 28
#define KEY_B_PIN 27

#define DEBOUNCE_TIME_KEYS 250

enum ENC_result { NONE, CH_UP, CH_DOWN };

static unsigned long last_interrupt_time = 0;
static ENC_result result = NONE;

void isr_encoder_keys() {
  uint32_t flags = save_and_disable_interrupts();
  //hw_clear_bits(&(iobank0_hw->proc0_irq_ctrl.inte[KEY_.... / 8]), 0b1111 << (KEY_.... % 8 )*4u );
   
  //SERIAL.println("in isr");
  if (millis()-last_interrupt_time>DEBOUNCE_TIME_KEYS) {
  
    if (digitalRead(KEY_A_PIN)==0){
	  result = CH_UP;
    }
      
    if (digitalRead(KEY_B_PIN)==0){
      result = CH_DOWN;
    }
      
    last_interrupt_time=millis();
  } // if (millis()-last_interrupt_time>DEBOUNCE_TIME_KEYS){
  
  restore_interrupts(flags); 
  //hw_set_bits(&(iobank0_hw->proc0_irq_ctrl.inte[KEY_.... / 8]), 0b0100 << (KEY_.... % 8 )*4u );
}


void gpiod_keys_init() {
  // put your setup code here, to run once:
  pinMode(KEY_A_PIN,INPUT_PULLUP);
  pinMode(KEY_B_PIN,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(KEY_A_PIN), isr_encoder_keys, FALLING);
  attachInterrupt(digitalPinToInterrupt(KEY_B_PIN), isr_encoder_keys, FALLING);
}

void gpiod_keys_run() {
  if (result == CH_UP)
    next_station();
    
  if (result == CH_DOWN)
    prev_station();
    
  result = NONE;
}
