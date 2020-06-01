#ifndef ram_hpp
#define ram_hpp

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "return_codes.h"
#include "font.h"

#define RAM_SIZE        4096        //4KB of RAM
#define LOAD_ADDRESS    0x200       //Start of program RAM

int InitRAM();
int DeInitRAM();
int LoadROM(char* filename);
unsigned long ReadUnsignedShort(unsigned short address);
unsigned char* GetRAMBuffer(unsigned short address);

#endif /* ram_hpp */
