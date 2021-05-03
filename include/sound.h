#ifndef SOUND_H
#define SOUND_H

void sound_init(void);
void sound_proc(void);

void sound_load_bgm(int id, const char *file);
void sound_play_bgm(int id);
void sound_stop_bgm(void);

void sound_load_bge(int id, const char *file);
void sound_play_bge(int id);
void sound_stop_bge(void);

void sound_load_sfx(int id, const char *file);
void sound_play_sfx(int id);

// stops looping and returns 1 when last queued buffer ends
int sound_request_bgm_to_end(void);

#endif
