#include <Arduino.h>
#include "globals.h" 

#include "hardware/structs/rtc.h"
#include "hardware/rtc.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"

#ifdef USE_ETHERNET

   #ifdef USE_ENC28J60
      #include <ENC28J60lwIP.h>
   #endif

   #ifdef USE_W5100
      #include <W5100lwIP.h>
   #endif
   
   #ifdef USE_W5500
      #include <W5500lwIP.h>
   #endif

#endif

#ifdef USE_WIFI
  #include <WiFi.h>
#endif


static boolean rtc_never_synced = true;
#define ASK_FOR_TIME_WHEN_NEVER_SYNCED_IN_MS 3000
#define ASK_FOR_TIME_TO_RESYNC_IN_MS (3600000) // resync time 
static unsigned long last_ask_for_time = 0;

bool resync_RTC() {
	time_t now = time(nullptr);

	SERIAL_PORT.printf("NTP_RTC: Current UTC-time (un-sync): %s", asctime(gmtime(&now)));  
	setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1); // see http://www.hs-help.net/hshelp/gira/v4_7/de/proj_tz.html
	tzset();
	SERIAL_PORT.printf("NTP_RTC: Current Localtime (un-sync): %s",asctime(localtime(&now)));  
	SERIAL_PORT.flush();
	    
	struct tm timeinfo;
	localtime_r(&now, &timeinfo);
    if (timeinfo.tm_year > 123) { //year 2023+ and not default-1970, so ntp res seems to be okay
      datetime_t t = { .year  = (int16_t) (1900 +timeinfo.tm_year), 
      				   .month = (int8_t)  (   1 +timeinfo.tm_mon),
      				   .day   = (int8_t) timeinfo.tm_mday, 
      				   .dotw  = (int8_t) timeinfo.tm_wday, 
       			       .hour  = (int8_t) timeinfo.tm_hour,
       			       .min   = (int8_t) timeinfo.tm_min ,
       			       .sec   = (int8_t) timeinfo.tm_sec  };
	  rtc_set_datetime(&t);
	  return true;
	}
	return false;
}



void ntp_rtc_init() {
  SERIAL_PORT.println("NTP_RTC: init");
  if (rtc_never_synced) {
    NTP.begin("pool.ntp.org", "time.nist.gov");
  }
}


void ntp_rtc_run() {
  if (rtc_never_synced) {
    if (millis()-last_ask_for_time > ASK_FOR_TIME_WHEN_NEVER_SYNCED_IN_MS) {
      if (resync_RTC()) 
    	rtc_never_synced = false;
    
	  last_ask_for_time = millis();
    } //  if (millis()-last_ask_for_time > ASK_FOR_TIME_WHEN_NEVER_SYNCED_IN_MS) {	     		      
  } else {
    if (millis()-last_ask_for_time > ASK_FOR_TIME_TO_RESYNC_IN_MS) {
      SERIAL_PORT.println("NTP_RTC: resync rtc with ntp"); SERIAL_PORT.flush();
      resync_RTC();
      last_ask_for_time = millis();
    }
  } //  else:if (rtc_never_synced) {
}
