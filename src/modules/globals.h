#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#define SERIAL_PORT Serial

//#define USE_SDCARD
#define USE_LITTLEFS

//#define USE_ETHERNET 
//#define USE_ENC28J60
//#define USE_W5100
//#define USE_W5500

#ifdef USE_ETHERNET
    #define SPI_PIN_RX  0
    #define SPI_PIN_CS  1
    #define SPI_PIN_SCK 2
    #define SPI_PIN_TX  3
#endif

#define USE_WIFI


#define USE_VS1053_DECODER
//#define USE_INTERNAL_CODEC_WITH_CUSTOM_LIB

// wiring VS1053-DSP
#define VS1053_CS     15   // VS1053 Chip Select Pin
#define VS1053_DCS     14   // VS1053 Data/Command Select Pin
#define VS1053_DREQ    13  // VS1053 Data request
#define VS1053_MOSI    12  // VS1053 MOSI
#define VS1053_MISO    11  // VS1053 MISO
#define VS1053_CLK     10   // VS1053 Clock Pin
#define VS1053_RESET   9   // VS1053 reset pin (-1 unbelegt)

//#define USE_INTERNAL_VU_METER
//#define USE_VLSI_VSDSP_VU_METER

// set by wifi.txt and playlist.m3u from SDCARD
#define MAX_CHANNELS_IN_LIST 100
extern char* playlist[MAX_CHANNELS_IN_LIST];
extern unsigned int channels_in_list;
extern unsigned int actual_channel_or_file_ID;

extern char ssid[32];           
extern char password[64];



#endif
