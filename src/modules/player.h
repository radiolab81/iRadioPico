#ifndef _PLAYER_H_
#define _PLAYER_H_

enum Player_State { STOPPED,   PREPARING,   RUNNING ,  PAUSED,	PREPAIRING_FAILED };

struct PlayerInfo {           
  unsigned int cur_ch_no;        
  char* cur_url_playing;
  int cur_buffer_level;
  Player_State cur_player_state;
  int cur_HTTP_RESPONSE = 0;
};  
 
void player_init();
void player_run();

void next_station();
void prev_station();
void pause_player();
void resume_player();
void stop_player();
void start_player();

PlayerInfo getPlayerInfo(void);

#endif
