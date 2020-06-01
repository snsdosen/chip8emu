#include "keyboard.h"

bool keys[KEY_COUNT];
bool newEntry = false;

int InitKeyboard(){
    memset(keys, 0, KEY_COUNT * sizeof(bool));
    return CODE_SUCCESS;
}

void SetKey(int keyIndex, bool keyState){
    keys[keyIndex] = keyState;
    if(keyState) newEntry = true;
}

bool GetKey(int keyIndex){
    return keys[keyIndex];
}

bool IsNewEntry(){
    return newEntry;
}

void ResetNewEntry(){
    newEntry = false;
}

void HandleInput(SDL_Event e){
    //Read input
    if(e.type == SDL_KEYDOWN){
        switch(e.key.keysym.sym){
            case SDLK_1:
                SetKey(0x1, true);
                break;
                
            case SDLK_2:
                SetKey(0x2, true);
                break;
                
            case SDLK_3:
                SetKey(0x3, true);
                break;
                
            case SDLK_4:
                SetKey(0xC, true);
                break;
                
            case SDLK_q:
                SetKey(0x4, true);
                break;
                
            case SDLK_w:
                SetKey(0x5, true);
                break;
                
            case SDLK_e:
                SetKey(0x6, true);
                break;
                
            case SDLK_r:
                SetKey(0xD, true);
                break;
                
            case SDLK_a:
                SetKey(0x7, true);
                break;
                
            case SDLK_s:
                SetKey(0x8, true);
                break;
                
            case SDLK_d:
                SetKey(0x9, true);
                break;
                
            case SDLK_f:
                SetKey(0xE, true);
                break;
                
            case SDLK_z:
            case SDLK_y:
                SetKey(0xA, true);
                break;
                
            case SDLK_x:
                SetKey(0x0, true);
                break;
                
            case SDLK_c:
                SetKey(0xB, true);
                break;
                
            case SDLK_v:
                SetKey(0xF, true);
                break;
        }
    }else if(e.type == SDL_KEYUP){
        switch(e.key.keysym.sym){
            case SDLK_1:
                SetKey(0x1, false);
                break;
                
            case SDLK_2:
                SetKey(0x2, false);
                break;
                
            case SDLK_3:
                SetKey(0x3, false);
                break;
                
            case SDLK_4:
                SetKey(0xC, false);
                break;
                
            case SDLK_q:
                SetKey(0x4, false);
                break;
                
            case SDLK_w:
                SetKey(0x5, false);
                break;
                
            case SDLK_e:
                SetKey(0x6, false);
                break;
                
            case SDLK_r:
                SetKey(0xD, false);
                break;
                
            case SDLK_a:
                SetKey(0x7, false);
                break;
                
            case SDLK_s:
                SetKey(0x8, false);
                break;
                
            case SDLK_d:
                SetKey(0x9, false);
                break;
                
            case SDLK_f:
                SetKey(0xE, false);
                break;
                
            case SDLK_z:
            case SDLK_y:
                SetKey(0xA, false);
                break;
                
            case SDLK_x:
                SetKey(0x0, false);
                break;
                
            case SDLK_c:
                SetKey(0xB, false);
                break;
                
            case SDLK_v:
                SetKey(0xF, false);
                break;
        }
    }
}
