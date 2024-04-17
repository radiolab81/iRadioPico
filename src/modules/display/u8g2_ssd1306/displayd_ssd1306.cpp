#include <Arduino.h>
#include <U8g2lib.h>
#include "../../globals.h"
#include "../../player.h"

#ifdef U8X8_HAVE_HW_SPI
	#include <SPI.h>
#endif

#ifdef U8X8_HAVE_HW_I2C
	#include <Wire.h>
#endif

#define I2C_SDA_PIN 20
#define I2C_SCL_PIN 21


#define FPS 10	// frames per second


static unsigned long last_call_time = 0;
#define FPS_SYNC_TIME 1000/FPS


static unsigned int url_lenght = 0;
static unsigned int cur_url_scroll_pos = 0;

// for other displays see https://github.com/olikraus/u8g2/wiki/u8g2setupcpp#constructor-reference
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);


void displayd_ssd1306_init() {
    Wire.setSDA(I2C_SDA_PIN);
    Wire.setSCL(I2C_SCL_PIN);
  
  	u8g2.begin();
  	u8g2.setFont(u8g2_font_6x10_tr);
  	u8g2.clearBuffer();
  	u8g2.setCursor(10, 32);
    u8g2.print("DISPLAYD init...");
    u8g2.sendBuffer();
 
}


void displayd_ssd1306_run() {

 	if (millis()-last_call_time > FPS_SYNC_TIME) {
		PlayerInfo info = getPlayerInfo();
		url_lenght = String(info.cur_url_playing).length();
	
		u8g2.clearBuffer();
		u8g2.setFont(u8g2_font_6x13B_tr);
		u8g2.setCursor(0, 15);
		u8g2.print("P"); u8g2.print(info.cur_ch_no); 
		u8g2.setCursor(100, 15);
		u8g2.print(info.cur_buffer_level); 
		u8g2.setCursor(120, 15);
		u8g2.print("%");
		u8g2.drawLine(0,20,128,20);
	
	    #ifdef USE_ETHERNET
	       	u8g2.setCursor(50, 15);
			u8g2.print("ETH");
	    #endif
	    
	    #ifdef USE_WIFI
	    	u8g2.setCursor(50, 15);
			u8g2.print("WiFi");
	    #endif 
	    
	    u8g2.setFont(u8g2_font_6x10_tr);
		u8g2.setCursor(0,40);
		
		if (cur_url_scroll_pos < url_lenght)
		  cur_url_scroll_pos++;
		else 
		  cur_url_scroll_pos=0;
		  
		u8g2.print(info.cur_url_playing+cur_url_scroll_pos);
		u8g2.sendBuffer();
	    
	    
	    last_call_time = millis();
	}	// 	if (millis()-last_call_time > FPS_SYNC_TIME) {
}
