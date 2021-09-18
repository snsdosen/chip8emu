#ifndef spu_h
#define spu_h

#include <SDL.h>
#include <SDL_audio.h>
#include <stdio.h>
#include "return_codes.h"

#define AMPLITUDE 28000
#define SAMPLE_RATE 44100

#define PLAY    0
#define STOP    1

int InitSPU();
void AudioCallback(void *user_data, Uint8 *raw_buffer, int bytes);
void PlaySPU(unsigned char duration);

#endif /* spu_h */
