#include "ram.h"

unsigned char* RAM = nullptr;      //Random Access Memory

//Initialize memory for usage
int InitRAM(){
    RAM = (unsigned char* )malloc(RAM_SIZE * sizeof(unsigned char));
    
    if(RAM == nullptr){
        printf("Failed to allocate %d bytes for emulated RAM\n", RAM_SIZE);
        return CODE_ERROR;
    }
    
    //Clear ram
    memset(RAM, 0, RAM_SIZE * sizeof(unsigned char));
    
    printf("Allocated %d bytes for emulated RAM\n", RAM_SIZE);
    return CODE_SUCCESS;
}

//Load ROM to RAM
int LoadROM(char* filename){
    SDL_RWops* file = SDL_RWFromFile( filename, "r+b" );
    Sint64 fileSize = 0;
    unsigned char* fileBuffer = nullptr;
    int loadOffset = 0;                     //Load offset for file header
    
    printf("Requested %s ROM\n", filename);
    
    //Unable to open file
    if( file == NULL ){
        printf( "Warning: Unable to open file! SDL Error: %s\n", SDL_GetError() );
        return CODE_ERROR;
    }
    
    fileSize = file->size(file);
    
    //Check if file is to big to be loaded in RAM
    if(fileSize > (RAM_SIZE - LOAD_ADDRESS)){
        printf("File is too big for emulated RAM\n");
        return CODE_ERROR;
    }

    printf("Filesize is: %lld\n", fileSize);

    //Allocate buffer
    fileBuffer = (unsigned char*) malloc(fileSize);
    if(fileBuffer == nullptr){
        printf("Failed to allocate file buffer");
        return CODE_ERROR;
    }
    
    SDL_RWread(file, fileBuffer, fileSize, 1);
    SDL_RWclose(file);
    
    //Flush any already loaded application
    memset(&RAM[LOAD_ADDRESS], 0, (RAM_SIZE - LOAD_ADDRESS) * sizeof(unsigned char));
    
    //Check if this file has a HP48 header ("HPHP48" magic)
    if(fileBuffer[0] == 0x48 && fileBuffer[1] == 0x50 && fileBuffer[2] == 0x48 && fileBuffer[3] == 0x50 && fileBuffer[4] == 0x34 && fileBuffer[5] == 0x38){
        printf("File contains HP48 header\n");
        return CODE_ERROR;
    }
    
    //Copy ROM to RAM
    memcpy(&RAM[LOAD_ADDRESS], fileBuffer + loadOffset, fileSize);
    
    printf("ROM loaded to RAM\n");
    
    return CODE_SUCCESS;
}

int DeInitRAM(){
    return CODE_SUCCESS;
}

unsigned long ReadUnsignedShort(unsigned short address){
    if(address > RAM_SIZE) return 0;        //Handle out of bounds address gracefully
    else return RAM[address] << 8 | RAM[address + 1];
}

unsigned char* GetRAMBuffer(unsigned short address){
    if(RAM == nullptr) return nullptr;
    else return &RAM[address];
}
