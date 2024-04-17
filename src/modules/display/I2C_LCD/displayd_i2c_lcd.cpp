#include <Arduino.h>
#include "../../globals.h"
#include "../../player.h"
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#define WIRE           Wire                                 // Wire  -> i2c0
//#define WIRE           Wire1                              // Wire1 -> i2c1

#define PIN_SDA 16                                           
#define PIN_SCL 17      

#define I2C_ADR_DISPLAY   0x3F                                   

LiquidCrystal_PCF8574 lcd(I2C_ADR_DISPLAY);

#define LCD_LINES      2                 
#define LCD_CHARS      16                 
#define BACKLIGHT      255                  


#define FPS 3	// frames per second


static unsigned long last_call_time = 0;
#define FPS_SYNC_TIME 1000/FPS


static unsigned int url_lenght = 0;
static unsigned int cur_url_scroll_pos = 0;

void displayd_i2c_lcd_init() {

  SERIAL_PORT.println("DISPLAYD: init I2C...");
  WIRE.setSDA(PIN_SDA);                        
  WIRE.setSCL(PIN_SCL);                                    
  WIRE.begin();
  
  SERIAL_PORT.println("DISPLAYD: Probing for PCF8574 ...");
  WIRE.beginTransmission(I2C_ADR_DISPLAY);                                     
   

  if (WIRE.endTransmission() == 0) {
    SERIAL_PORT.println("DISPLAYD: LCD found.");
    lcd.begin(LCD_CHARS, LCD_LINES);
    lcd.setBacklight(BACKLIGHT);
    lcd.home(); lcd.clear();
    lcd.print("DISPLAYD init...");

  } else {
    SERIAL_PORT.println("DISPLAYD: LCD not found.");
  } 
  
}

void displayd_i2c_lcd_run() {

	if (millis()-last_call_time > FPS_SYNC_TIME) {
		PlayerInfo info = getPlayerInfo();
		url_lenght = String(info.cur_url_playing).length();

		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("P"); lcd.print(info.cur_ch_no);
		
		#ifdef USE_ETHERNET
	       	lcd.setCursor(5, 0);
			lcd.print("ETH");
	    #endif
	    
	    #ifdef USE_WIFI
	    	lcd.setCursor(5, 0);
			lcd.print("WiFi");
	    #endif 
	    
	    lcd.setCursor(11,0);
	    lcd.print(info.cur_buffer_level);
	    lcd.print("%"); 

	    if (cur_url_scroll_pos < url_lenght)
		  cur_url_scroll_pos++;
		else 
		  cur_url_scroll_pos=0;
		  
		lcd.setCursor(0,1);
		lcd.print(info.cur_url_playing+cur_url_scroll_pos);
		
		last_call_time = millis();
    }   // 	if (millis()-last_call_time > FPS_SYNC_TIME) {

}
