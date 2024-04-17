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

#define FPS 10	// frames per second

static unsigned long last_call_time = 0;
#define FPS_SYNC_TIME 1000/FPS


static unsigned int url_lenght = 0;
static unsigned int cur_url_scroll_pos = 0;

static Arduino_DataBus  *bus = NULL;
static Arduino_GFX		*gfx = NULL;
static Arduino_GFX      *canvas  = NULL; // use a canvas instead of direct gfx-drawing, to prevent display flickering

void displayd_st7735_init() { 
    SERIAL_PORT.println("DISPLAYD: st7735 init");

   	// for other databus see https://github.com/moononournation/Arduino_GFX/tree/master/src/databus
   	bus = new Arduino_RPiPicoSPI(PIN_DC_A0, PIN_CS, PIN_SCK, PIN_MOSI, PIN_MISO , spi0 /* spi0/1 */);   // hardware SPI or
   	//Arduino_DataBus *bus = new Arduino_SWSPI(PIN_DC_A0 , PIN_CS , PIN_SCK , PIN_MOSI , GFX_NOT_DEFINED /* MISO */); // default software SPI
   	
    // for other displays see https://github.com/moononournation/Arduino_GFX/blob/master/src/display/
    /*Arduino_ST7735(
      Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0,
      bool ips = false, int16_t w = ST7735_TFTWIDTH, int16_t h = ST7735_TFTHEIGHT,
      uint8_t col_offset1 = 0, uint8_t row_offset1 = 0, uint8_t col_offset2 = 0, uint8_t row_offset2 = 0,
      bool bgr = true);*/
   	//Arduino_ST7735 *gfx = new Arduino_ST7735(bus);
   	if (bus != NULL) {
   		gfx = new Arduino_ST7735(bus, PIN_RST, 3, false ,128,160, 0, 0, 0, 0,false);
   	} else {
   	  SERIAL_PORT.println("DISPLAYD: st7735 bus failed");
   	  exit(-1);
   	}
   	
   	if (gfx != NULL) {
		canvas = new Arduino_Canvas(160 /* width */, 128 /* height */, gfx);
	} else {
   	  SERIAL_PORT.println("DISPLAYD: st7735 gfx failed");
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


void displayd_st7735_run() {  /* see great Arduino_GFX lib doc for all graphic commands/primitives */
	
	if (millis()-last_call_time > FPS_SYNC_TIME) {
		PlayerInfo info = getPlayerInfo();
		url_lenght = String(info.cur_url_playing).length();
		
		if (canvas!=NULL) {
			canvas->fillScreen(BLACK);
			canvas->setTextColor(YELLOW);
			canvas->setCursor(0, 5);
			canvas->print("P"); canvas->print(info.cur_ch_no); 
			canvas->setCursor(120, 5);
			canvas->print(info.cur_buffer_level); 
			canvas->setCursor(140, 5);
			canvas->print("%");
			canvas->writeFastHLine(0,20,160,20);
	
	    #ifdef USE_ETHERNET
	       	canvas->setCursor(50, 5);
			canvas->print("ETH");
	    #endif
	    
	    #ifdef USE_WIFI
	    	canvas->setCursor(50, 5);
			canvas->print("WiFi");
	    #endif 
	        
	        canvas->writeFastHLine(0,70,160,70);
	        canvas->setCursor(5, 80);
	        canvas->print("HTTP-Status-Code:"); canvas->print(info.cur_HTTP_RESPONSE);
	        
	    	canvas->setTextColor(GREEN);
			canvas->setCursor(0,25);
		
			if (cur_url_scroll_pos < url_lenght)
		  	  cur_url_scroll_pos++;
			else 
		  	  cur_url_scroll_pos=0;
		  
		    canvas->print(info.cur_url_playing+cur_url_scroll_pos);
		    canvas->flush();

		}
	    
	    last_call_time = millis();
	}	// 	if (millis()-last_call_time > FPS_SYNC_TIME) {
}

