#include <Arduino.h>
#include "src/modules/globals.h" 

#ifdef USE_SDCARD
   #include "src/modules/sdcard.h"
#endif

#ifdef USE_LITTLEFS
   #include "src/modules/lfs.h"
#endif

#include "src/modules/player.h"

#include "src/modules/gpiod_rotary.h"
#include "src/modules/gpiod_keys.h"
#include "src/modules/gesture/gpiod_gesture.h"
#include "src/modules/gpiod_potentiometer.h"

//#include "src/modules/display/I2C_LCD/displayd_i2c_lcd.h"
#include "src/modules/display/u8g2_ssd1306/displayd_ssd1306.h"
//#include "src/modules/display/Arduino_GFX_st7735/displayd_st7735.h"
//#include "src/modules/display/baby_metz/baby_metz.h"
//#include "src/modules/display/Arduino_GFX_ILI9341/displayd_ILI9341.h"


#ifdef USE_ETHERNET

   #ifdef USE_ENC28J60
      #include <ENC28J60lwIP.h>
      ENC28J60lwIP eth(1 /* chip select */);
   #endif

   #ifdef USE_W5100
      #include <W5100lwIP.h>
      #ifdef ARDUINO_WIZNET_5100S_EVB_PICO
         // To use Interrupt-driven mode, pass in an SPI object and an IRQ pin like so:
         Wiznet5100lwIP eth(17, SPI, 21);
      #else
         Wiznet5100lwIP eth(1 /* chip select */);
      #endif
   #endif
   
   #ifdef USE_W5500
      #include <W5500lwIP.h>
      Wiznet5500lwIP eth(1 /* chip select */);
   #endif

#endif

#ifdef USE_WIFI
  #include <WiFi.h>
#endif

static unsigned long global_heartbeat_counter = 0;

//********************************************************************
// thats our "rc.local" to start all services we need on cpu_core_0
// Att: network related stuff (ETH, WiFi libs) works only on core 0
//********************************************************************
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
    
  SERIAL_PORT.begin(115200);
  /*while (!SERIAL_PORT) {
    delay(1);  // wait for serial port to connect. Needed for native USB port only
  }*/
  
  
  // PLAYLIST init
  #ifdef USE_SDCARD
    while(readPlaylistSDCard()!= SD_OK) {
        delay(5000);
        SERIAL_PORT.println("insert SDCARD with playlist.m3u to continue ...");
    }
  #endif

  #ifdef USE_LITTLEFS
    while(readPlaylistLittleFS()!= LFS_OK) {
        delay(5000);
        SERIAL_PORT.println("upload playlist.m3u to LittleFS to continue ...");
    }
  #endif

  // NETWORK HARDWARE init
  #ifdef USE_ETHERNET
    // Set up SPI pinout to match your HW
    SPI.setRX(SPI_PIN_RX);
    SPI.setCS(SPI_PIN_CS);
    SPI.setSCK(SPI_PIN_SCK);
    SPI.setTX(SPI_PIN_TX);

    // see https://arduino-pico.readthedocs.io/_/downloads/en/latest/pdf/  Chapter: ETHERNETLWIP (WIRED ETHERNET) SUPPORT 
    eth.setSPISpeed(30000000);
    lwipPollingPeriod(3);
    
    // Start the Ethernet port
    if (!eth.begin()) {
      SERIAL_PORT.println("No wired Ethernet hardware detected. Check pinouts, wiring.");
      while (1) {
        delay(1000);
      }
    }

    while (!eth.connected()) {
      SERIAL_PORT.print(".");
      delay(500);
    }

    SERIAL_PORT.println("");
    SERIAL_PORT.println("Ethernet connected");
    SERIAL_PORT.println("IP address: ");
    SERIAL_PORT.println(eth.localIP());
    
  #endif

  #ifdef USE_WIFI
     #ifdef USE_SDCARD
        if (readWifiConfigSDCard()==SD_OK) {
     #endif
     
     #ifdef USE_LITTLEFS
        if (readWifiConfigLittleFS()==LFS_OK) {
     #endif    
          WiFi.mode(WIFI_STA);
          WiFi.setHostname("iRadioPico");
          SERIAL_PORT.printf("Connecting to '%s' with '%s'\n", ssid, password);
          WiFi.begin(ssid, password);
       
          while (WiFi.status() != WL_CONNECTED) {
             SERIAL_PORT.print(".");
             delay(100);
          }
          SERIAL_PORT.printf("\nConnected to WiFi at %s\n", WiFi.localIP().toString().c_str());
       }
  #endif //   #ifdef USE_WIFI


  // START (your own) TASKs: PLAYER, DISPLAYD, GPIOD, ... on cpu0
  player_init();
  gpiod_rotary_init(); 
  //gpiod_keys_init();
  //gpiod_gesture_init(); 
  //gpiod_potentiometer_init();
}


//********************************************************************
// cpu_0 runner
// Att: network related stuff (ETH, WiFi libs) works only on core 0
//********************************************************************
void loop() {
   
   player_run();
   gpiod_rotary_run();
   //gpiod_keys_run();
   //gpiod_gesture_run();
   //gpiod_potentiometer_run();
}



void task_heartbeat()
{
     SERIAL_PORT.println("iRadioPico: Heartbeat");
     digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); 
}

//********************************************************************
// thats our "rc.local" to start all services we need on cpu_core_1
// Att: network related stuff (ETH, WiFi libs) works only on core 0
//********************************************************************
void setup1() {
  //displayd_i2c_lcd_init();
  displayd_ssd1306_init();
  //displayd_st7735_init();
  //displayd_baby_metz_init();
  //displayd_ILI9341_init();
  
}

//********************************************************************
// cpu_1 runner
// Att: network related stuff (ETH, WiFi libs) works only on core 0
//********************************************************************
void loop1() {
  //displayd_i2c_lcd_run();
  displayd_ssd1306_run();
  //displayd_st7735_run();
  //displayd_baby_metz_run();
  //displayd_ILI9341_run();

  global_heartbeat_counter++;
  if (global_heartbeat_counter%1330000 == 0)
     task_heartbeat();
  
}
