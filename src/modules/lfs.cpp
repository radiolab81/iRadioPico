#include "globals.h"
#include "lfs.h"

#include <Arduino.h>

#include "LittleFS.h"

int readWifiConfigLittleFS(){
   SERIAL_PORT.println("\nLITTLEFS: Initializing LittleFS...");
    
   if (!LittleFS.begin()){
     Serial.println(F("LITTLEFS: initialization failed!"));
     return LFS_FAILURE;
   }
   SERIAL_PORT.println("LITTLEFS: initialization done.");;
  
   File dir = LittleFS.open("/", "r");   
   if (LittleFS.exists("wifi.txt")) {
     SERIAL_PORT.println("LITTLEFS: wifi.txt exists.");
     File fd = LittleFS.open("wifi.txt","r");
     if (fd) {
	   while (fd.available()) {
          String line = fd.readStringUntil('\n');  // \n character is discarded from buffer
     	  
     	  // SSID
     	  if ( line.indexOf("ssid=") != -1) {
     	     SERIAL_PORT.print("LITTLEFS: SSID found: ");
     	     String data = line.substring(line.indexOf("ssid=")+6,line.indexOf("\"", line.indexOf("ssid=")+6));
     	     memcpy(ssid,data.c_str(),data.length());
     	     SERIAL_PORT.println(ssid);
     	  }
     	  
     	  // PSK
     	  if ( line.indexOf("psk=") != -1) {
     	     SERIAL_PORT.print("LITTLEFS: PSK found: ");
     	     String data = line.substring(line.indexOf("psk=")+5,line.indexOf("\"", line.indexOf("psk=")+5));
     	     memcpy(password,data.c_str(),data.length());
     	     SERIAL_PORT.println(password);
     	  }
       } //  while (fd.available()) {
       fd.close();
     } // if (fd) {
   } else {
         SERIAL_PORT.println("LITTLEFS: wifi.txt doesn't exist.");
         return LFS_FAILURE_NO_FILE;
   }

  return LFS_OK;
}

int readPlaylistLittleFS() {      
  SERIAL_PORT.println("\nLITTLEFS: Initializing LittleFS...");
    
  if (!LittleFS.begin()){
      Serial.println(F("LITTLEFS: initialization failed!"));
      return LFS_FAILURE;
  }
  SERIAL_PORT.println("LITTLEFS: initialization done.");
         
  File dir = LittleFS.open("/", "r");   
  if (LittleFS.exists("/playlist.m3u")) {
    SERIAL_PORT.println("LITTLEFS: playlist exists.");
    File fd = LittleFS.open("playlist.m3u","r");
    if (fd) {
      unsigned int line_count = 0;
      while ((fd.available()) && (line_count < MAX_CHANNELS_IN_LIST)) {
        String url = fd.readStringUntil('\n');  // \n character is discarded from buffer
        if ((url.length()>0) && (url.length()<255)) {
          playlist[line_count]=(char*) malloc(255*sizeof(char));
          url.toCharArray(playlist[line_count], url.length()+1);
          line_count++;
        
          SERIAL_PORT.print("LITTLEFS: playlist at line ");
          SERIAL_PORT.print(line_count);
          SERIAL_PORT.print(" : ");
          SERIAL_PORT.println(url);
        } // if ((url.length()>0) && (url.length()<255)) {
      } // while ((fd.available()) && (line_count < MAX_CHANNELS_IN_LIST)) {
      channels_in_list = line_count;
      fd.close();
    } // if (fd) {
  } else {
    SERIAL_PORT.println("LITTLEFS: playlist doesn't exist.");
    return LFS_FAILURE_NO_FILE;
  }
  return LFS_OK;
} 