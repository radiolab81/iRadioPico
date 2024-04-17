#ifndef _SDCARD_H_
#define _SDCARD_H_

#include "globals.h"

#define  SD_OK				0
#define  SD_FAILURE_SPI 	1
#define  SD_FAILURE_NO_FILE	2
#define  SD_FAILURE_CARD	3

/**
 * @brief      Create Wi-Fi peripheral configuration from wifi.txt on sdcard. 
 *
 * @param      void
 *
 * @return     The Wi-Fi peripheral configuration. 
 */

int readWifiConfigSDCard();


/**
 * @brief      Create channel list from playlist.m3u on sdcard. 
 *
 * @param      void
 *
 * @return     SD_OK,SD_FAILURE_SPI,SD_FAILURE_NO_FILE
 */
int readPlaylistSDCard();


#endif
