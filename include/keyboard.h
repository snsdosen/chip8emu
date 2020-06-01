#ifndef keyboard_h
#define keyboard_h

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "return_codes.h"

#define KEY_COUNT   16

int InitKeyboard();
void SetKey(int keyIndex, bool keyState);
bool GetKey(int keyIndex);
bool IsNewEntry();
void ResetNewEntry();
void HandleInput(SDL_Event e);

#endif /* keyboard_h */
