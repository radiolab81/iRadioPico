#include "globals.h"
#include "sdcard.h"

#include <Arduino.h>

#include <SPI.h>
#include <SD.h>

#include "LittleFS.h"

/*

  This example shows how read the files in a
  directory on a SD card

  The circuit:
   SD card attached to SPI bus as follows on RP2040:
   ************ SPI0 ************
   ** MISO (AKA RX) - pin 0, 4, or 16
   ** MOSI (AKA TX) - pin 3, 7, or 19
   ** CS            - pin 1, 5, or 17
   ** SCK           - pin 2, 6, or 18
   ************ SPI1 ************
   ** MISO (AKA RX) - pin  8 or 12
   ** MOSI (AKA TX) - pin 11 or 15
   ** CS            - pin  9 or 13
   ** SCK           - pin 10 or 14

*/

// This are GP pins for SPI0 on the Raspberry Pi Pico board, and connect
// to different *board* level pinouts.  Check the PCB while wiring.
// Only certain pins can be used by the SPI hardware, so if you change
// these be sure they are legal or the program will crash.
// See: https://datasheets.raspberrypi.com/picow/PicoW-A4-Pinout.pdf
const int _MISO = 4;  
const int _MOSI = 7; 
const int _CS = 5;
const int _SCK = 6;



int readWifiConfigSDCard() {
  SERIAL_PORT.println("\nSDCARD: Initializing SD card...");

  bool sdInitialized = false;
  // Ensure the SPI pinout the SD card is connected to is configured properly
  // Select the correct SPI based on _MISO pin for the RP2040
  if (_MISO == 0 || _MISO == 4 || _MISO == 16) {
    SPI.setRX(_MISO);
    SPI.setTX(_MOSI);
    SPI.setSCK(_SCK);
    sdInitialized = SD.begin(_CS);
  } else if (_MISO == 8 || _MISO == 12) {
    SPI1.setRX(_MISO);
    SPI1.setTX(_MOSI);
    SPI1.setSCK(_SCK);
    sdInitialized = SD.begin(_CS, SPI1);
  } else {
    SERIAL_PORT.println(F("ERROR: SDCARD: Unknown SPI Configuration"));
    return SD_FAILURE_SPI;
  }

  if (!sdInitialized) {
    SERIAL_PORT.println("SDCARD: initialization failed!");
    return SD_FAILURE_CARD;
  }
  SERIAL_PORT.println("SDCARD: initialization done.");
  
   if (SD.exists("wifi.txt")) {
     SERIAL_PORT.println("SDCARD: wifi.txt exists.");
     File fd = SD.open("wifi.txt",FILE_READ);
     if (fd) {
	   while (fd.available()) {
          String line = fd.readStringUntil('\n');  // \n character is discarded from buffer
     	  
     	  // SSID
     	  if ( line.indexOf("ssid=") != -1) {
     	     SERIAL_PORT.print("SDCARD: SSID found: ");
     	     String data = line.substring(line.indexOf("ssid=")+6,line.indexOf("\"", line.indexOf("ssid=")+6));
     	     memcpy(ssid,data.c_str(),data.length());
     	     SERIAL_PORT.println(ssid);
     	  }
     	  
     	  // PSK
     	  if ( line.indexOf("psk=") != -1) {
     	     SERIAL_PORT.print("SDCARD: PSK found: ");
     	     String data = line.substring(line.indexOf("psk=")+5,line.indexOf("\"", line.indexOf("psk=")+5));
     	     memcpy(password,data.c_str(),data.length());
     	     SERIAL_PORT.println(password);
     	  }
       } //  while (fd.available()) {
       fd.close();
       if (_MISO == 0 || _MISO == 4 || _MISO == 16) {
         SPI.end();
       } else if (_MISO == 8 || _MISO == 12) {
         SPI1.end();
       } 
       SERIAL_PORT.println("SDCARD: SPI/SPI1 closed for further use");
     } // if (fd) {
   } else {
         SERIAL_PORT.println("SDCARD: wifi.txt doesn't exist.");
         return SD_FAILURE_NO_FILE;
   }
  
  return SD_OK;
}


int readPlaylistSDCard() {      
  SERIAL_PORT.println("\nSDCARD: Initializing SD card...");
  
  bool sdInitialized = false;
  // Ensure the SPI pinout the SD card is connected to is configured properly
  // Select the correct SPI based on _MISO pin for the RP2040
  if (_MISO == 0 || _MISO == 4 || _MISO == 16) {
    SPI.setRX(_MISO);
    SPI.setTX(_MOSI);
    SPI.setSCK(_SCK);
    sdInitialized = SD.begin(_CS);
  } else if (_MISO == 8 || _MISO == 12) {
    SPI1.setRX(_MISO);
    SPI1.setTX(_MOSI);
    SPI1.setSCK(_SCK);
    sdInitialized = SD.begin(_CS, SPI1);
  } else {
    SERIAL_PORT.println(F("ERROR: SDCARD: Unknown SPI Configuration"));
    return SD_FAILURE_SPI;
  }

  if (!sdInitialized) {
    SERIAL_PORT.println("SDCARD: initialization failed!");
    return SD_FAILURE_CARD;
  }
  SERIAL_PORT.println("SDCARD: initialization done.");

  if (SD.exists("playlist.m3u")) {
    SERIAL_PORT.println("SDCARD: playlist exists.");
    File fd = SD.open("playlist.m3u",FILE_READ);
    if (fd) {
      unsigned int line_count = 0;
      while ((fd.available()) && (line_count < MAX_CHANNELS_IN_LIST)) {
        String url = fd.readStringUntil('\n');  // \n character is discarded from buffer
        if ((url.length()>0) && (url.length()<255)) {
          playlist[line_count]=(char*) malloc(255*sizeof(char));
          url.toCharArray(playlist[line_count], url.length()+1);
          line_count++;
        
          SERIAL_PORT.print("SDCARD: playlist at line ");
          SERIAL_PORT.print(line_count);
          SERIAL_PORT.print(" : ");
          SERIAL_PORT.println(url);
        } // if ((url.length()>0) && (url.length()<255)) {
      } // while ((fd.available()) && (line_count < MAX_CHANNELS_IN_LIST)) {
      channels_in_list = line_count;
      fd.close();
      if (_MISO == 0 || _MISO == 4 || _MISO == 16) {
        SPI.end();
      } else if (_MISO == 8 || _MISO == 12) {
        SPI1.end();
      }
      SERIAL_PORT.println("SDCARD: SPI/SPI1 closed for further use");
    } // if (fd) {
  } else {
    SERIAL_PORT.println("SDCARD: playlist doesn't exist.");
    return SD_FAILURE_NO_FILE;
  }
  return SD_OK;
} 

