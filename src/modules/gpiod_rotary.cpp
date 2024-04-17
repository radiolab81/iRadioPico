#include <Arduino.h>
#include "player.h"

/* small and lousy rotary encoder code (but works with external deb.caps/LPF and shielding!) to show channel switching,
youre free to add your own code for better detection and multiple encoders */
 
#define ENCODER_1_PIN_A 27
#define ENCODER_1_PIN_B 28

static int currentStateCLK;
static int lastStateCLK;

void gpiod_rotary_init() {

	pinMode(ENCODER_1_PIN_A,INPUT_PULLUP);
	pinMode(ENCODER_1_PIN_B,INPUT_PULLUP); 

}
void gpiod_rotary_run() {       
	// Read the current state of CLK
	currentStateCLK = digitalRead(ENCODER_1_PIN_A);

	// If last and current state of CLK are different, then pulse occurred
	// React to only 1 state change to avoid double count
	if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){

		// If the DT state is different than the CLK state then
		// the encoder is rotating CCW so decrement
		if (digitalRead(ENCODER_1_PIN_B) != currentStateCLK) {
			next_station();
		} else {
			// Encoder is rotating CW so increment
			prev_station();
		}

	}
	
	// Remember last CLK state
	lastStateCLK = currentStateCLK;
}
