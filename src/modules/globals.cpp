#include "globals.h"

// set by wifi.txt,playlist.m3u,settings.txt from SDCARD/LFS

char* playlist[MAX_CHANNELS_IN_LIST];
unsigned int channels_in_list;
unsigned int actual_channel_or_file_ID;

char ssid[32];           
char password[64];

unsigned int volume_L, volume_R;
