#include "spu.h"

int sample_nr = 0;
Uint32 startTicks = 0;
Uint32 endTicks = 0;

void AudioCallback(void *user_data, Uint8 *raw_buffer, int bytes){
    Uint32 currentTicks = SDL_GetTicks();
    Sint16 *buffer = (Sint16*)raw_buffer;
    int length = bytes / 2; // 2 bytes per sample for AUDIO_S16SYS
    int &sample_nr(*(int*)user_data);
    
    for(int i = 0; i < length; i++, sample_nr++)
    {
        double time = (double)sample_nr / (double)SAMPLE_RATE;
        buffer[i] = (Sint16)(AMPLITUDE * sin(2.0f * M_PI * 441.0f * time)); // render 441 HZ sine wave
    }
    
    //Stop playback after elapsed time
    if(currentTicks >= endTicks) SDL_PauseAudio(STOP);
}

int InitSPU(){
    SDL_AudioSpec fmt;
    fmt.freq = SAMPLE_RATE;
    fmt.format = AUDIO_S16SYS;
    fmt.channels = 1;
    fmt.samples = 2048;
    fmt.callback = AudioCallback;
    fmt.userdata = &sample_nr;
    
    SDL_AudioSpec native_fmt;
    if(SDL_OpenAudio(&fmt, &native_fmt) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to open audio: %s", SDL_GetError());
        return CODE_ERROR;
    }
    if(fmt.format != native_fmt.format){
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to get the desired AudioSpec");
        return CODE_ERROR;
    }

    return CODE_SUCCESS;
}

void PlaySPU(unsigned char duration){
    startTicks = SDL_GetTicks();
    endTicks = SDL_GetTicks() + duration * 16;
    SDL_PauseAudio(PLAY);
}
