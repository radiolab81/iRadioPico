#ifndef _LFS_H_
#define _LFS_H_

#include "globals.h"

#define  LFS_OK				 0
#define  LFS_FAILURE_NO_FILE 1
#define  LFS_FAILURE		 2


/**
 * @brief      Create Wi-Fi peripheral configuration from wifi.txt on littlefs. 
 *
 * @param      void
 *
 * @return     The Wi-Fi peripheral configuration. 
 */

int readWifiConfigLittleFS();


/**
 * @brief      Create channel list from playlist.m3u on littlefs. 
 *
 * @param      void
 *
 * @return     LFS_OK,LFS_FAILURE,LFS_FAILURE_NO_FILE
 */
int readPlaylistLittleFS();


#endif
