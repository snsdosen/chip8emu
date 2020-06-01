#include <SDL2/SDL.h>
#include <stdio.h>

#include "app_info.h"
#include "return_codes.h"
#include "ram.h"
#include "gpu.h"
#include "cpu.h"
#include "spu.h"
#include "keyboard.h"

bool InitSDL();
void DeInitSDL();

unsigned int lastTime = 0, currentTime = 0;
unsigned int frameNum = 0;

SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };

const int IPS = 750;        //Instructions per second
const int HIGH_IPS = 1000;  //Instructions per second in HIGH (Super Chip) mode

//CPU is not running by default
bool KillSwitch = true;

bool InitSDL()
{
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return CODE_ERROR;
    }

    return CODE_SUCCESS;
}

void DeInitSDL()
{
    //Quit SDL subsystems
    SDL_Quit();
}

int main( int argc, char* args[] )
{
    //Initialize SDL
    if(InitSDL() != CODE_SUCCESS){
        printf( "Failed to initialize!\n" );
        return CODE_ERROR;
    }

    //Get info on the attached display
    if(SDL_GetDisplayMode(0, 0, &mode) != 0){
        printf("SDL_GetDisplayMode failed: %s", SDL_GetError());
        return CODE_ERROR;
    }

    //Init all relevant systems
    if(InitRAM() != CODE_SUCCESS) return CODE_ERROR;
    if(InitCPU() != CODE_SUCCESS) return CODE_ERROR;
    if(InitGPU() != CODE_SUCCESS) return CODE_ERROR;
    if(InitSPU() != CODE_SUCCESS) return CODE_ERROR;
    if(InitKeyboard() != CODE_SUCCESS) return CODE_ERROR;

    //Place chip8 font in the memory
    CopyFontToMemory();

    printf("IPF: %d\n", IPS / mode.refresh_rate);

    //Enable drop event for drag and drop
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

    //Main loop flag
    bool quit = false;

    //Event handler
    SDL_Event e;

    //Start with a clean CPU state
    ResetCPU();

    //While application is running
    while( !quit )
    {
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            //Handle emulated keyboard input
            HandleInput(e);

            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
            //Special keys
            else if(e.type == SDL_KEYDOWN){
                switch(e.key.keysym.sym){
                    case SDLK_ESCAPE:
                        quit = true;
                        break;

                    case SDLK_l:
                        DumpRegisters();
                        break;

                    case SDLK_o:
                        //RunCPU(1);
                        break;

                    case SDLK_p:
                        ResetGPU();
                        SetVideoMode(MODE_LOW);
                        ResetCPU();
                        break;
                }
            }
            else if(e.type == SDL_DROPFILE){
                if(LoadROM(e.drop.file) == CODE_SUCCESS){
                    ResetCPU();
                    SetVideoMode(MODE_LOW);
                    ResetGPU();

                    //Allow execution
                    KillSwitch = false;
                }
            }
        }

        //Run CPU only if it's not crashed
        if(!KillSwitch){
            if(RunCPU(IPS/mode.refresh_rate, HIGH_IPS/mode.refresh_rate) != CODE_SUCCESS) KillSwitch = true;
        }

        UpdateScreen();

        frameNum++;

        //Calculate framerate
        currentTime = SDL_GetTicks();
        if (currentTime > lastTime + 1000) {
            //SetWindowTitle(frameNum);
            lastTime = currentTime;

            frameNum = 0;
        }
    }

    //Free resources and close SDL
    DeInitSDL();

    return CODE_SUCCESS;
}
