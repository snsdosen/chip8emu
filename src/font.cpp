#include "font.h"

void CopyFontToMemory(){
    memcpy(GetRAMBuffer(0), &chip8_fontset, 80);
    memcpy(GetRAMBuffer(80), &super_chip8_fontset, 100);
}

