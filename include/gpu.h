#ifndef gpu_h
#define gpu_h

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "return_codes.h"
#include "app_info.h"

#define SCREEN_WIDTH        64
#define SCREEN_HEIGHT       32

#define HIGH_SCREEN_WIDTH   128
#define HIGH_SCREEN_HEIGHT  64

#define SCREEN_SCALE        5

#define MODE_LOW            0       //Low res mode
#define MODE_HIGH           1       //High res mode

#define SCROLL_LEFT         0
#define SCROLL_RIGHT        1
#define SCROLL_DOWN         2

int InitGPU();
void ResetGPU();
bool PlotPixel(unsigned char X, unsigned char Y);
bool Draw(unsigned char Xcoor, unsigned char Ycoor, unsigned char* buffer, char length);
void UpdateScreen();
void SetWindowTitle(char* title);
void SetVideoMode(int mode);
int GetVideoMode();
void ScrollDisplay(int mode, int pixels);
void DrawFromFB();

#endif /* gpu_h */
