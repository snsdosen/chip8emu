#include "gpu.h"

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* mTexture = NULL;

bool* VRAM = nullptr;
int VideoMode = MODE_LOW;

int InitGPU(){
    VideoMode = MODE_LOW;
    
    //Create window
    gWindow = SDL_CreateWindow( APP_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, HIGH_SCREEN_WIDTH * SCREEN_SCALE, HIGH_SCREEN_HEIGHT * SCREEN_SCALE, SDL_WINDOW_SHOWN );
    if(gWindow == NULL){
        printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
        return CODE_ERROR;
    }
    
    //Create renderer for the window
    gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(gRenderer == NULL){
        printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
        return CODE_ERROR;
    }
    
    //Create blank textures
    mTexture = SDL_CreateTexture( gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, HIGH_SCREEN_WIDTH, HIGH_SCREEN_HEIGHT );
    if(mTexture == NULL){
        printf( "Unable to create blank texture! SDL Error: %s\n", SDL_GetError() );
        return CODE_ERROR;
    }
    
    //Allocate memory for VRAM
    VRAM = (bool*) malloc(HIGH_SCREEN_WIDTH * HIGH_SCREEN_HEIGHT * sizeof(bool));
    
    if(VRAM == nullptr){
        printf("Failed to allocate %d bytes for emulated VRAM\n", HIGH_SCREEN_WIDTH * HIGH_SCREEN_HEIGHT);
        return CODE_ERROR;
    }
    
    //Clear VRAM
    memset(VRAM, 0, HIGH_SCREEN_WIDTH * HIGH_SCREEN_HEIGHT * sizeof(bool));
    
    printf("Allocated %d bytes for emulated VRAM\n", HIGH_SCREEN_WIDTH * HIGH_SCREEN_HEIGHT);
    
    //Clear screen
    ResetGPU();
    
    return CODE_SUCCESS;
}

bool PlotPixel(unsigned char X, unsigned char Y){
    bool flagRaised = false;
    
    //Prohibit drawing outside of the framebuffer
    if(X >= HIGH_SCREEN_WIDTH || Y >= HIGH_SCREEN_HEIGHT) return false;
    
    if(VRAM[(X) + (Y * HIGH_SCREEN_WIDTH)]){
        flagRaised = true;
        VRAM[(X) + (Y * HIGH_SCREEN_WIDTH)] = false;
    }else{
        VRAM[(X) + (Y * HIGH_SCREEN_WIDTH)] = true;
    }

    return flagRaised;
}

//Return state of the flag register
bool Draw(unsigned char Xcoor, unsigned char Ycoor, unsigned char* buffer, char length){
    int spriteLength = length;
    int byteLen = 1;
    int byteIndex = 0;
    
    //16x16 sprite in HIGH and 8x16 in LOW mode
    if(length == 0){
        spriteLength = 16;
        if(VideoMode == MODE_HIGH) byteLen = 2;
    }
    
    bool flagRaised = false;
    
    unsigned char X = Xcoor % HIGH_SCREEN_WIDTH;
    unsigned char Y = Ycoor % HIGH_SCREEN_HEIGHT;
    
    if(VideoMode == MODE_LOW){
        X = Xcoor % SCREEN_WIDTH;
        Y = Ycoor % SCREEN_HEIGHT;
    }
    
    for(int j=0; j < spriteLength; j++){
        
        for(int b = 0; b < byteLen; b++){
            for(int i = 0; i < 8; i++){
                if((buffer[byteIndex] & (0x80 >> i)) > 0) {
                    if(VideoMode == MODE_LOW){
                        if(PlotPixel((X + i) * 2, (Y + j) * 2)) flagRaised = true;
                        if(PlotPixel((X + i) * 2 + 1, (Y + j) * 2)) flagRaised = true;
                        if(PlotPixel((X + i) * 2, (Y + j) * 2 + 1)) flagRaised = true;
                        if(PlotPixel((X + i) * 2 + 1, (Y + j) * 2 + 1)) flagRaised = true;
                    }else{
                        if(PlotPixel(X + i + (b * 8), Y + j)) flagRaised = true;
                    }
                }
            }
            
            byteIndex++;
        }
        
    }
    
    return flagRaised;
}

void UpdateScreen(){
    if(gRenderer == NULL || mTexture == NULL) return;
    
    DrawFromFB();
    
    //Draw to screen
    SDL_SetRenderTarget( gRenderer, NULL );
    SDL_RenderCopy(gRenderer, mTexture, NULL, NULL);
    SDL_RenderPresent( gRenderer );
}

void SetWindowTitle(char* title){
    if(gWindow != NULL) SDL_SetWindowTitle(gWindow, title);
}

void ResetGPU(){
    SDL_SetRenderTarget(gRenderer, mTexture);
    SDL_SetRenderDrawColor( gRenderer, 0x0, 0x0, 0x0, 0xFF );
    SDL_RenderClear(gRenderer);
    SDL_RenderPresent( gRenderer );
    
    //Clear VRAM
    memset(VRAM, 0, HIGH_SCREEN_WIDTH * HIGH_SCREEN_HEIGHT * sizeof(bool));
}

void SetVideoMode(int mode){
    VideoMode = mode;
}

int GetVideoMode(){
    return VideoMode;
}

void ScrollDisplay(int mode, int pixels){
    unsigned char tempBuffer[HIGH_SCREEN_WIDTH];
    
    for(int i = 0; i < HIGH_SCREEN_HEIGHT - pixels; i++){
        switch(mode){
            default:
                memcpy(tempBuffer, &VRAM[(HIGH_SCREEN_HEIGHT - 1 - i) * HIGH_SCREEN_WIDTH], HIGH_SCREEN_WIDTH);
                memset(&VRAM[(HIGH_SCREEN_HEIGHT - 1 - i) * HIGH_SCREEN_WIDTH], 0, HIGH_SCREEN_WIDTH);
                
                if(mode == SCROLL_LEFT){
                    memcpy(&VRAM[(HIGH_SCREEN_HEIGHT - 1 - i) * HIGH_SCREEN_WIDTH], tempBuffer + pixels, HIGH_SCREEN_WIDTH - pixels);
                }
                else if(mode == SCROLL_RIGHT){
                    memcpy(&VRAM[(HIGH_SCREEN_HEIGHT - 1 - i) * HIGH_SCREEN_WIDTH + pixels], tempBuffer, HIGH_SCREEN_WIDTH - pixels);
                }
                break;

            case SCROLL_DOWN:
                memcpy(&VRAM[(HIGH_SCREEN_HEIGHT - 1 - i) * HIGH_SCREEN_WIDTH], &VRAM[(HIGH_SCREEN_HEIGHT - 1 - pixels - i) * HIGH_SCREEN_WIDTH], HIGH_SCREEN_WIDTH);
                memset(&VRAM[(HIGH_SCREEN_HEIGHT - 1 - pixels - i) * HIGH_SCREEN_WIDTH], 0, HIGH_SCREEN_WIDTH);  //Erase old line
                break;
        }
    }
}

//Draw display from framebuffer
void DrawFromFB(){
    SDL_SetRenderTarget(gRenderer, mTexture);
    
    for(int j = 0; j < HIGH_SCREEN_HEIGHT; j++){
        for(int i = 0; i < HIGH_SCREEN_WIDTH; i++){
            if(VRAM[(HIGH_SCREEN_WIDTH * j) + i] == true) {
                SDL_SetRenderDrawColor(gRenderer, 153, 255, 153, 0xFF);
                SDL_RenderDrawPoint(gRenderer, i, j);
            }else{
                SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x0, 0xFF);
                SDL_RenderDrawPoint(gRenderer, i, j);
            }
        }
    }
}
