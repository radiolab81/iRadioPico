#include "globals.h"
#include "player.h"
#include <Arduino.h>

#include <HTTPClient.h>
#include "QueueArray.h"

#ifdef USE_LITTLEFS
   #include "lfs.h"
   
   #ifdef USE_AUTO_SAVE
       #include "autosave.h"
   #endif
#endif

#ifdef USE_VS1053_DECODER

	#include <Adafruit_VS1053.h>
	
	Adafruit_VS1053 VS1053Dekoder = Adafruit_VS1053(VS1053_MOSI, VS1053_MISO, VS1053_CLK, VS1053_RESET, VS1053_CS, VS1053_DCS, VS1053_DREQ);

	WiFiClient client;
	HTTPClient http;
	WiFiClient *stream = NULL;
	
	uint16_t pcm_value_left = 0, pcm_value_right = 0;
	
	#ifdef USE_VLSI_VSDSP_VU_METER
	   #include "vs1053b_patches.h"
	   uint8_t vsdsp_vu_left, vsdsp_vu_right;
	   uint16_t vu_dsp_both;
	#endif
	
#endif


#ifdef USE_INTERNAL_CODEC_WITH_CUSTOM_LIB

    // add your includes and code here

#endif


class Buffer32Byte {
public:
	uint8_t data[32];
};

#define BUF_SIZE 2048
QueueArray <Buffer32Byte *> queue(BUF_SIZE); // Puffer für Streaming

Player_State state = STOPPED;
int http_get_response = 0;

Player_Event playerEvent = NO_EVENT;

void terminate_audioplayer_pipeline() {
  SERIAL_PORT.println("PLAYER: terminate audio pipeline");
  
  state = STOPPED;
  
  #ifdef USE_VS1053_DECODER  
  
  	stream = NULL;
  	// clear buffer mem
  	while (!queue.isEmpty()) {
          delete(queue.pop());
  	}
  	 
    //http.end(); -> same issue like https://github.com/espressif/arduino-esp32/issues/828
    client.stop();
    client.flush();
  
  #endif
  
  #ifdef USE_INTERNAL_CODEC_WITH_CUSTOM_LIB
  
    // add your code for termination of audio pipeline here

  #endif
}


void create_audioplayer_pipeline(int channel_num) {
  state = PREPARING;
  SERIAL_PORT.println("PLAYER: create a new audio pipeline");
    
  String new_url;
  String new_host;
  int port = 80;
  if ((channel_num >= 0) && (channel_num < channels_in_list) ) {
     new_url = playlist[channel_num];
  } else {
     new_url = playlist[0];
  }

  new_host = new_url.substring(new_url.indexOf("://")+3,new_url.indexOf("/", new_url.indexOf("://")+3));
  
  if (new_host.lastIndexOf(":") != -1) {
     String new_port = new_host.substring(new_host.lastIndexOf(":")+1,new_host.length());
     new_host = new_host.substring(0,new_host.lastIndexOf(":"));
     port = new_port.toInt();
  }

  SERIAL_PORT.print("PLAYER: connect to ");
  SERIAL_PORT.print(new_host); SERIAL_PORT.print(" : Port "); SERIAL_PORT.println(port);
  
  #ifdef USE_VS1053_DECODER
  	// Use WiFiClient class to create TCP connections
  	if (!client.connect(new_host, port)) {
    	  SERIAL_PORT.println("PLAYER: connection failed, try next one");
    	  state = PREPAIRING_FAILED;
    	  delay(1000);
  	}

  	// do a GET request
  	http.setInsecure();  // Use certs, but do not check their authenticity
  	http.begin(client, new_url);
  	http.setReuse(true);
  	http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  	http_get_response = http.GET();
  	if (http_get_response != HTTP_CODE_OK) {
    	  http.end();
    	  SERIAL_PORT.println("PLAYER: Can't open HTTP request");
    	  state = STOPPED;
  	}
  	SERIAL_PORT.print("PLAYER: server response code is ");
  	SERIAL_PORT.println(http_get_response); 
 
  	stream = http.getStreamPtr();
  #endif //  #ifdef USE_VS1053_DECODER
  
  #ifdef USE_INTERNAL_CODEC_WITH_CUSTOM_LIB
  
    // add your audio pipeline init code here

  #endif
  
  state = RUNNING;
}

  
void player_init() {
   state = STOPPED;
   
   #ifdef USE_VS1053_DECODER
		
	   if (!VS1053Dekoder.begin()) { // initialisiere VS1053-Dekoder
     	 SERIAL_PORT.println("PLAYER: no VS1053-DSP detected");
     	 while (1) delay(1000);
  	   }			

   	   SERIAL_PORT.println("PLAYER: VS1053-DSP found");
	   
	   #ifdef USE_VLSI_VSDSP_VU_METER   
	      // load vs1053 apps and patches from vs1053b_patches.h
	      VS1053Dekoder.applyPatch(plugin, PLUGIN_SIZE);
	      
	      /*VU Meter -SCI_STATUS bit 9 enables VU meter, which calculates a leaking peak sample value 
	      from both channels and returns the values in 1 dB resolution through SCI_AICTRL3.
	      The high 8 bits represent the left channel, the low 8 bits the right channel */
  	      VS1053Dekoder.sciWrite(0x01, VS1053Dekoder.sciRead(0x01) | (1<<9) );  // SCI_STATUS(0x01)      
	   #endif
	   	
	   // start with programm no 0
	   actual_channel_or_file_ID = 0;
	   // start volume
   	   volume_L = 10, volume_R = 10;   	   
	
	      
	   #ifdef USE_LITTLEFS
          #ifdef USE_AUTO_SAVE
             readRadioSettingsLittleFS();  // actual_channel_or_file_ID, volume_L/R ... overwritten by settings.txt 
          #endif
       #endif
	
       VS1053Dekoder.setVolume(volume_L, volume_R);
	   
       if (radiomode == INTERNETRADIO)
	     create_audioplayer_pipeline(actual_channel_or_file_ID);
	         
   #endif
   
   
   #ifdef USE_INTERNAL_CODEC_WITH_CUSTOM_LIB
   
   	 // add your once running player ini code here
       
   #endif
}

void player() {
  if (state == RUNNING) {

  	#ifdef USE_VS1053_DECODER
  
       float fuellstand = (queue.count() / BUF_SIZE) * 99;
       //SERIAL_PORT.println(fuellstand);
            
  	   if (VS1053Dekoder.readyForData()) {
   	     if (!queue.isEmpty()) {
     		Buffer32Byte *coded_audio = queue.pop();
     		VS1053Dekoder.playData(coded_audio->data, 32);
     		delete(coded_audio);
  	     } 
   	   } 
  			 
  	#endif
  	
  	 	
  	#ifdef USE_INTERNAL_CODEC_WITH_CUSTOM_LIB
      
  	#endif 
  } //  if (state == RUNNING) { 
}

void playerFillBufferTask()
{
     #ifdef USE_VS1053_DECODER
        if (state == RUNNING) {
          if (stream!=NULL) {
            if (stream->available() > 32) {
              if (!queue.isFull()) {
                Buffer32Byte *coded_audio = new Buffer32Byte();
                stream->read(coded_audio->data, 32);
                queue.push(coded_audio);
              } 
            }	 
          } 
        }
     #endif
  
     #ifdef USE_INTERNAL_CODEC_WITH_CUSTOM_LIB
  
    	// add code for buffer filling here
     
     #endif
}

void player_run(){
     #ifdef USE_VS1053_DECODER
        if (playerEvent == VOLUME_CHANGED) {
          SERIAL_PORT.println("PLAYER: VOLUME_CHANGED event (att in 0.5 dB steps) L/R=" + String(volume_L) + "/" + String(volume_R));
          playerEvent = NO_EVENT;
      	  VS1053Dekoder.setVolume(volume_L, volume_R);
      	}
      	
      	#ifdef USE_INTERNAL_VU_METER
        	// read DAC registers for left channel
      		VS1053Dekoder.sciWrite(VS1053_REG_WRAMADDR,0xC015);
      		pcm_value_left = VS1053Dekoder.sciRead(VS1053_REG_WRAM);
      	
      		// read DAC registers for right channel
      		VS1053Dekoder.sciWrite(VS1053_REG_WRAMADDR,0xC016);
      		pcm_value_right = VS1053Dekoder.sciRead(VS1053_REG_WRAM);
      	#endif
      	
      	#ifdef USE_VLSI_VSDSP_VU_METER
      	    /*VU Meter - SCI_STATUS bit 9 enables VU meter, which calculates a leaking peak sample value
      	    from both channels and returns the values in 1 dB resolution through SCI_AICTRL3. 
      	    The high 8 bits represent the left channel, the low 8 bits the right channel.
      	    Note that the following has changed from the previous release.
      	    Values from 0 to 96 are valid for both channels. 0 means no sound, 96 is highest level.
      	    In practise the highest value will be 95, the signal is probably clipping if you get 96.
      	    The calculation does not take the volume setting into account.
      	    The VU meter takes about 0.6 MHz of processing power with 48 kHz samplerate. */
      	    
      	    vu_dsp_both = VS1053Dekoder.sciRead(0x0F);  // SCI_AICTRL3 
      	    vsdsp_vu_left = ((uint8_t) vu_dsp_both);
      	    vsdsp_vu_right = ((uint8_t) (vu_dsp_both>>8));
      	#endif
	
        playerFillBufferTask();
     #endif
	
     player();
}

void goto_station(int ch){
  SERIAL_PORT.print("PLAYER: goto_station "); SERIAL_PORT.println(ch);
  if ((state == RUNNING) || (state == PREPAIRING_FAILED) || (state == PREPARING) ){
    if ( (ch >=0) && (ch < channels_in_list ) ) {
      actual_channel_or_file_ID=ch;
    
      terminate_audioplayer_pipeline();
      create_audioplayer_pipeline(actual_channel_or_file_ID);
    }
  }
}

void next_station() {
  SERIAL_PORT.println("PLAYER: next_station");
  if ((state == RUNNING) || (state == PREPAIRING_FAILED) || (state == PREPARING) ){
    if (actual_channel_or_file_ID < (channels_in_list-1))
      actual_channel_or_file_ID++;
    else 
      actual_channel_or_file_ID=0;
  
    terminate_audioplayer_pipeline();
    create_audioplayer_pipeline(actual_channel_or_file_ID);
  }
}

void prev_station() {
  SERIAL_PORT.println("PLAYER: prev_station");
  if ((state == RUNNING) || (state == PREPAIRING_FAILED) || (state == PREPARING)  ) {
    if (actual_channel_or_file_ID > 0)
      actual_channel_or_file_ID--;
    else
      actual_channel_or_file_ID= (channels_in_list-1);
  
    terminate_audioplayer_pipeline();
    create_audioplayer_pipeline(actual_channel_or_file_ID);
  }
}
  
void pause_player() {
 if (state == RUNNING) { 
   SERIAL_PORT.println("PLAYER: paused");
   state = PAUSED;
 }
}

void resume_player() {
  if (state == PAUSED) {
   SERIAL_PORT.println("PLAYER: resume");
   state = RUNNING;
  }
}

void stop_player() {
  if (state == RUNNING)  {
    SERIAL_PORT.println("PLAYER: stopping");
    state = STOPPED;
    terminate_audioplayer_pipeline();
  } 
}

void start_player() {
  if (state == STOPPED) {
    SERIAL_PORT.println("PLAYER: starting");
    create_audioplayer_pipeline(actual_channel_or_file_ID);
  }
}
  
/* for VS1053 see
9.6.11 SCI_VOL (RW)
SCI_VOL is a volume control for the player hardware. The most significant byte of the volume
register controls the left channel volume, the low part controls the right channel volume. The
channel volume sets the attenuation from the maximum volume level in 0.5 dB steps. Thus,
maximum volume is 0x0000 and total silence is 0xFEFE. 
Setting SCI_VOL to 0xFFFF will activate analog powerdown mode. */
void volume_down() {
   #ifdef USE_VS1053_DECODER
 	if ( volume_L < 250 ) 
	  volume_L+=5;
	
  	if ( volume_R < 250)
	  volume_R+=5;
    
	playerEvent = VOLUME_CHANGED;
   #endif
}

void volume_up() {
   #ifdef USE_VS1053_DECODER
	if ( volume_L > 5 ) 
	  volume_L-=5;
	
	if ( volume_R > 5 )
  	  volume_R-=5;
			
  	playerEvent = VOLUME_CHANGED;
   #endif
}

  
PlayerInfo getPlayerInfo(void) {
   PlayerInfo info;
   
   info.cur_ch_no = actual_channel_or_file_ID;
   info.cur_url_playing = playlist[actual_channel_or_file_ID];
   info.cur_buffer_level = int((queue.count() / (float)BUF_SIZE) * 100);
     
   info.cur_player_state = state;
   info.cur_HTTP_RESPONSE = http_get_response;
   
   #ifdef USE_VS1053_DECODER
   		info.pcm_value_left = pcm_value_left;
   		info.pcm_value_right = pcm_value_right;
   #endif
   
   #ifdef USE_VLSI_VSDSP_VU_METER
        info.vsdsp_vu_left = vsdsp_vu_left;
        info.vsdsp_vu_right = vsdsp_vu_right;
   #endif
   
   return info;
}

