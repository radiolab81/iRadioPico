#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#include "../../globals.h"
#include "../../player.h"

#define PIN_DC_A0 21
#define PIN_RST 20
#define PIN_CS 5
#define PIN_SCK 2
#define PIN_MOSI 3
#define PIN_MISO GFX_NOT_DEFINED

#define FPS 20	// frames per second

static unsigned long last_call_time = 0;
#define FPS_SYNC_TIME 1000/FPS


static unsigned int url_lenght = 0;
static unsigned int cur_url_scroll_pos = 0;

static Arduino_DataBus  *bus = NULL;
static Arduino_GFX	*gfx = NULL;
static Arduino_GFX      *canvas  = NULL; // use a canvas instead of direct gfx-drawing, to prevent display flickering

#ifdef USE_INTERNAL_VU_METER
static uint8_t vu_L, vu_R;
static unsigned short linToDBTab[5] = {36781, 41285, 46341, 52016, 58386};
/*
Converts a linear 16-bit value between 0..65535 to decibels.
Reference level: 32768 = 96dB (largest VS1053b number is 32767 = 95dB).
Bugs:
- For the input of 0, 0 dB is returned, because minus infinity cannot
be represented with integers.
- Assumes a ratio of 2 is 6 dB, when it actually is approx. 6.02 dB.
see; https://www.vlsi.fi/fileadmin/software/VS10XX/VS1053_VS1063_PcmRecorder.pdf  ,2.4.3 Building a Useful VU Meter
*/
unsigned short LinToDB(unsigned short n) {
	int res = 96, i;
	if (!n) /* No signal should return minus infinity */
	  return 0;

	while (n < 32768U) { /* Amplify weak signals */
	  res -= 6;
	  n <<= 1;
	}
	
	for (i=0; i<5; i++) /* Find exact scale */
	  if (n > linToDBTab[i])
	    res++;
	    
	return res;
}
#endif

void displayd_ILI9341_init() { 
    SERIAL_PORT.println("DISPLAYD: ILI9341 init");

   	// for other databus see https://github.com/moononournation/Arduino_GFX/tree/master/src/databus
   	bus = new Arduino_RPiPicoSPI(PIN_DC_A0, PIN_CS, PIN_SCK, PIN_MOSI, PIN_MISO , spi0 /* spi0/1 */);   // hardware SPI or
   	//Arduino_DataBus *bus = new Arduino_SWSPI(PIN_DC_A0 , PIN_CS , PIN_SCK , PIN_MOSI , GFX_NOT_DEFINED /* MISO */); // default software SPI
   	
    	// for other displays see https://github.com/moononournation/Arduino_GFX/blob/master/src/display/
    	/*:Arduino_ILI9341(Arduino_DataBus *bus, int8_t rst, uint8_t r, bool ips)
    	: Arduino_TFT(bus, rst, r, ips, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, 0, 0, 0, 0)*/
   	if (bus != NULL) {
   		gfx = new Arduino_ILI9341(bus, PIN_RST, 3, false);
   	} else {
   	  SERIAL_PORT.println("DISPLAYD: ILI9341 bus failed");
   	  exit(-1);
   	}
   	
   	if (gfx != NULL) {
   	    canvas = new Arduino_Canvas_Indexed(320, 240, gfx); // vy slow, but Arduino_Canvas/ILI9341 causes WiFi side effects -> tbo.
	} else {
   	  SERIAL_PORT.println("DISPLAYD: ILI9341 gfx failed");
   	  exit(-1);
   	}

	// Init Display
  	if (!canvas->begin()) {
    	SERIAL_PORT.println("canvas->begin() failed!");
    	exit(-1);
  	}
	
    if (canvas != NULL) {
		canvas->fillScreen(BLACK);
		canvas->setTextColor(WHITE);
		canvas->setTextSize(1);
		canvas->setCursor(30, 64);
		canvas->println("DISPLAYD init...");
		canvas->flush();
	}
}


void displayd_ILI9341_run() {  /* see great Arduino_GFX lib doc for all graphic commands/primitives */
	
	if (millis()-last_call_time > FPS_SYNC_TIME) {
		PlayerInfo info = getPlayerInfo();
		url_lenght = String(info.cur_url_playing).length();
		
		if (canvas!=NULL) {
			canvas->fillScreen(BLACK);
			canvas->setTextSize(2);
			canvas->setTextColor(YELLOW);
			canvas->setCursor(0, 5);
			canvas->print("P"); canvas->print(info.cur_ch_no); 
			canvas->setCursor(260, 5);
			canvas->print(info.cur_buffer_level); 
			canvas->setCursor(300, 5);
			canvas->print("%");
			canvas->writeFastHLine(0,30,320,20);
	
	    #ifdef USE_ETHERNET
	       		canvas->setCursor(150, 5);
			canvas->print("ETH");
	    #endif
	    
	    #ifdef USE_WIFI
	    		canvas->setCursor(150, 5);
			canvas->print("WiFi");
	    #endif 
	        
	        	canvas->writeFastHLine(0,210,320,210);
	        	canvas->setCursor(5, 220);
	       	 	canvas->print("HTTP-Status-Code:"); canvas->print(info.cur_HTTP_RESPONSE);
	        
	    		canvas->setTextColor(GREEN);
			canvas->setCursor(0,50);
		
			if (cur_url_scroll_pos < url_lenght)
		  	  cur_url_scroll_pos++;
			else 
		  	  cur_url_scroll_pos=0;
		  
		    	canvas->print(info.cur_url_playing+cur_url_scroll_pos);
		    
		    #ifdef USE_INTERNAL_VU_METER
		    	canvas->setCursor(0,100);
		    	vu_L = (uint8_t) LinToDB(info.pcm_value_left);
		    	canvas->print("L="); 
		    	canvas->drawRect(24,  99,  101, 17, WHITE);
		    	canvas->fillRect(25, 100, vu_L, 15, GREEN);
		    
		    	canvas->setCursor(0,120);
		    	vu_R = (uint8_t) LinToDB(info.pcm_value_right);
		    	canvas->print("R=");
		    	canvas->drawRect(24, 119,  101, 17, WHITE);
		    	canvas->fillRect(25, 120, vu_R, 15, GREEN);
		    
		    #endif
		    
		    	canvas->flush();

		} // if (canvas!=NULL) {
	    
	    last_call_time = millis();
	}	// 	if (millis()-last_call_time > FPS_SYNC_TIME) {
}
