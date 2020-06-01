#include "cpu.h"

bool halted = false;

unsigned char registers[REGISTER_COUNT];
unsigned short stack[STACK_COUNT];
unsigned char hp48flags[HP48FLAGS_COUNT];

unsigned short  IReg = 0;
unsigned char   VFReg = 0;

unsigned char delayTimer = 0;

unsigned short  programCounter = LOAD_ADDRESS;
unsigned char   stackPointer = 0;

//For jump instructions which do not need to increment program counter
bool skipPCIncrement = false;

SDL_TimerID timerID;

void ResetCPU(){
    memset(registers, 0, REGISTER_COUNT * sizeof(unsigned char));
    memset(hp48flags, 0, HP48FLAGS_COUNT * sizeof(unsigned char));
    memset(stack, 0, STACK_COUNT * sizeof(unsigned short));
    
    IReg = 0;
    VFReg = 0;
    
    delayTimer = 0;
    
    programCounter = LOAD_ADDRESS;
    stackPointer = 0;
    
    halted = false;
}

int InitCPU(){
    //srand (time(0));
    srand( static_cast<unsigned int>(time(nullptr)));
    
    //Start clean
    ResetCPU();
    
    //Setup SDL timer for sound and delay timers
    timerID = SDL_AddTimer( TIMER_INTERVAL, timerCallback, NULL );
    
    return CODE_SUCCESS;
}

Uint32 timerCallback( Uint32 interval, void* param ){
    if(delayTimer > 0) delayTimer--;

    //Timer needs to be recurring
    return TIMER_INTERVAL;
}

//Execute a single opcode fetched from RAM
int RunOpcode(unsigned short opcode){
    unsigned char opc = opcode >> 12;
    
    if(halted) return CODE_HALT;
    
    skipPCIncrement = false;
    
    //printf("OPC: 0x%X\n", opcode);
    
    switch(opc){
        default:
            UnknownOpcode(opcode);
            return CODE_ERROR;
            break;
        
        case 0x0:
            OPC0(opcode);
            break;
            
        case 0x1:
            OPC1(opcode);
            break;
            
        case 0x2:
            OPC2(opcode);
            break;
            
        case 0x3:
            OPC3(opcode);
            break;
        
        case 0x4:
            OPC4(opcode);
            break;
        
        case 0x5:
            OPC5(opcode);
            break;
            
        case 0x6:
            OPC6(opcode);
            break;
            
        case 0x7:
            OPC7(opcode);
            break;
        
        case 0x8:
            OPC8(opcode);
            break;
        
        case 0x9:
            OPC9(opcode);
            break;
            
        case 0xA:
            OPCA(opcode);
            break;
            
        case 0xB:
            OPCB(opcode);
            break;
            
        case 0xC:
            OPCC(opcode);
            break;
            
        case 0xD:
            OPCD(opcode);
            break;
            
        case 0xE:
            OPCE(opcode);
            break;
            
        case 0xF:
            OPCF(opcode);
            break;
    }
    
    
    //printf("Opcode lookup: %x\n", opc);
    
    //Go to next instruction
    if(!skipPCIncrement){
        if(IncrementProgramCounter() != CODE_SUCCESS) return CODE_ERROR;
    }
    
    return CODE_SUCCESS;
}

int IncrementProgramCounter(){
    if(programCounter < RAM_SIZE) {
        programCounter += 2;
        return CODE_SUCCESS;
    }else{
        printf("Program counter outside of address space\n");
        return CODE_ERROR;
    }
}

//Start CPU execution
int RunCPU(int instructionCount, int highInstructionCount){
    int IPC = instructionCount;
    if(GetVideoMode() == MODE_HIGH) IPC = highInstructionCount;
    
    for(int i = 0; i < IPC; i++){
        if(RunOpcode(ReadUnsignedShort(programCounter)) != CODE_SUCCESS){
            printf("CPU halted\n");
            halted = true;
            return CODE_HALT;
        };
    }
    return CODE_SUCCESS;
}

void UnknownOpcode(unsigned short opcode){
    printf("Unknown opcode: %X\n", opcode);
}

unsigned char GetXRegister(unsigned short opcode){
    return 0xF & (opcode >> 8);
}

unsigned char GetYRegister(unsigned short opcode){
    return 0xF & (opcode >> 4);
}

unsigned char GetByte(unsigned short opcode){
    return opcode & 0xFF;
}

unsigned short GetAddress(unsigned short opcode){
    return opcode & 0x0FFF;
}

unsigned char GetNibble(unsigned short opcode){
    return opcode & 0xF;
}

//Opcode handlers
void OPC0(unsigned short opcode){
    switch (GetByte(opcode)) {
        //CLS
        case 0xE0:
            ResetGPU();
            break;
        
        //RET
        case 0xEE:
            stackPointer--;
            programCounter = stack[stackPointer];
            break;
        
        //SCR
        case 0xFB:
            ScrollDisplay(SCROLL_RIGHT, 4);
            break;
            
        //SCL
        case 0xFC:
            ScrollDisplay(SCROLL_LEFT, 4);
            break;
            
        //HALT
        case 0xFD:
            halted = true;
            break;
            
        //LOW
        case 0xFE:
            SetVideoMode(MODE_LOW);
            break;
            
        //HIGH
        case 0xFF:
            SetVideoMode(MODE_HIGH);
            break;
            
        default:
            //Special super chip opcode, SCD
            if((GetByte(opcode) & 0xF0) == 0xC0){
                ScrollDisplay(SCROLL_DOWN, GetNibble(opcode));
            }else UnknownOpcode(opcode);
            break;
    }
}

//JP addr
void OPC1(unsigned short opcode){
    programCounter = GetAddress(opcode);
    skipPCIncrement = true;
}

//CALL addr
void OPC2(unsigned short opcode){
    stack[stackPointer] = programCounter;
    stackPointer++;
    OPC1(opcode);
}

//SE Vx, byte
void OPC3(unsigned short opcode){
    if(registers[GetXRegister(opcode)] == GetByte(opcode)) IncrementProgramCounter();
}

//SNE Vx, byte
void OPC4(unsigned short opcode){
    if(registers[GetXRegister(opcode)] != GetByte(opcode)) IncrementProgramCounter();
}

//SE Vx, Vy
void OPC5(unsigned short opcode){
    if(registers[GetXRegister(opcode)] == registers[GetYRegister(opcode)]) IncrementProgramCounter();
}

//LD Vx, byte
void OPC6(unsigned short opcode){
    registers[GetXRegister(opcode)] = GetByte(opcode);
}

//ADD Vx, byte
void OPC7(unsigned short opcode){
    registers[GetXRegister(opcode)] += GetByte(opcode);
}

void OPC8(unsigned short opcode){
    switch (GetNibble(opcode)) {
            
        //LD Vx, Vy
        case 0:
            registers[GetXRegister(opcode)] = registers[GetYRegister(opcode)];
            break;
            
        //OR Vx, Vy
        case 1:
            registers[GetXRegister(opcode)] |= registers[GetYRegister(opcode)];
            break;
            
        //AND Vx, Vy
        case 2:
            registers[GetXRegister(opcode)] &= registers[GetYRegister(opcode)];
            break;
            
        //XOR Vx, Vy
        case 3:
            registers[GetXRegister(opcode)] ^= registers[GetYRegister(opcode)];
            break;
            
        case 4:
            ADD(opcode);
            break;
        
        case 5:
            SUB(opcode);
            break;
            
        case 6:
            SHR(opcode);
            break;
            
        case 7:
            SUBN(opcode);
            break;
            
        case 0xE:
            SHL(opcode);
            break;
            
        default:
            UnknownOpcode(opcode);
            break;
    }
}

//SNE Vx, Vy
void OPC9(unsigned short opcode){
    if(registers[GetXRegister(opcode)] != registers[GetYRegister(opcode)]) IncrementProgramCounter();
}

//LD I, addr
void OPCA(unsigned short opcode){
    IReg = GetAddress(opcode);
}

//JP V0, addr
void OPCB(unsigned short opcode){
    programCounter = GetAddress(opcode) + registers[0];
}

void OPCC(unsigned short opcode){
    registers[GetXRegister(opcode)] = GetByte(opcode) & (rand() % 0xFF);
}

//DRW Vx, Vy, nibble
void OPCD(unsigned short opcode){
    if(Draw(registers[GetXRegister(opcode)], registers[GetYRegister(opcode)], GetRAMBuffer(IReg), GetNibble(opcode))){
        registers[0xF] = 1; //Collision
    }else{
        registers[0xF] = 0; //No collision
    }
}

void OPCE(unsigned short opcode){
    switch (GetByte(opcode)) {
        default:
            UnknownOpcode(opcode);
            break;
            
        //SKP Vx
        case 0x9E:
            if(GetKey(registers[GetXRegister(opcode)])) IncrementProgramCounter();
            break;
            
        case 0xA1:
            if(!GetKey(registers[GetXRegister(opcode)])) IncrementProgramCounter();
            break;
    }
}

void OPCF(unsigned short opcode){
    switch (GetByte(opcode)) {
        //LD Vx, DT
        case 0x07:
            registers[GetXRegister(opcode)] = delayTimer;
            break;
        
        //LD Vx, K
        case 0x0A:
            skipPCIncrement = true;
            for(int i=0; i < KEY_COUNT; i++){
                if(GetKey(i) && IsNewEntry()){
                    skipPCIncrement = false;
                    registers[GetXRegister(opcode)] = i;
                    ResetNewEntry();
                    break;
                }
            }
            break;
            
        //LD DT, Vx
        case 0x15:
            delayTimer = registers[GetXRegister(opcode)];
            break;
        
        //LD ST, Vx
        case 0x18:
            PlaySPU(registers[GetXRegister(opcode)]);
            break;
            
        //ADD I, Vx
        case 0x1E:
            IReg += registers[GetXRegister(opcode)];
            
            //Check for overflow
            if(IReg > 0xFFF){
                registers[0xF] = 1;
                IReg &= 0xFFF;
            }else{
                registers[0xF] = 0;
            }
            break;
        
        //LD F, Vx
        case 0x29:
            IReg = registers[GetXRegister(opcode)] * CHARACTER_LENGTH;
            break;
            
        //LD HF, Vx
        case 0x30:
            IReg = 80 + registers[GetXRegister(opcode)] * HIGH_CHARACTER_LENGTH;
            break;
        
        //LD B, Vx
        case 0x33:
            DEC2BCD(registers[GetXRegister(opcode)]);
            break;
            
        //LD [I], Vx
        case 0x55:
            memcpy(GetRAMBuffer(IReg), registers, GetXRegister(opcode) + 1);
            break;
            
        //LD Vx, [I]
        case 0x65:
            memcpy(registers, GetRAMBuffer(IReg), GetXRegister(opcode) + 1);
            break;
        
        case 0x75:
            memcpy(hp48flags, registers, GetXRegister(opcode) + 1);
            break;
            
        case 0x85:
            memcpy(registers, hp48flags, GetXRegister(opcode) + 1);
            break;
            
        default:
            UnknownOpcode(opcode);
            break;
    }
}

//Convert decimal to Binary Coded Decimal
void DEC2BCD(unsigned char number){
    unsigned char* buffer = GetRAMBuffer(IReg);
    
    buffer[0] = number / 100;           //ones
    buffer[1] = (number / 10) % 10;     //tens
    buffer[2] = (number % 100) % 10;    //hundreds
}

void ADD(unsigned short opcode){
    unsigned short result = registers[GetXRegister(opcode)] + registers[GetYRegister(opcode)];
    if (result > 0xFF) registers[0xF] = 1; else registers[0xF] = 0;
    registers[GetXRegister(opcode)] = result & 0xFF;
}

void SUB(unsigned short opcode){
    //VF is a carry and zero flag in this opcode, fixes ant
    if(registers[GetXRegister(opcode)] >= registers[GetYRegister(opcode)]) registers[0xF] = 1; else registers[0xF] = 0;
    registers[GetXRegister(opcode)] -= registers[GetYRegister(opcode)];
}

void SHR(unsigned short opcode){
    registers[0xF] = registers[GetXRegister(opcode)] & 0x1;
    registers[GetXRegister(opcode)] >>= 1;
}

void SUBN(unsigned short opcode){
    //VF is a carry and zero flag
    if(registers[GetYRegister(opcode)] >= registers[GetXRegister(opcode)]) registers[0xF] = 1; else registers[0xF] = 0;
    registers[GetXRegister(opcode)] = registers[GetYRegister(opcode)] - registers[GetXRegister(opcode)];
}

void SHL(unsigned short opcode){
    registers[0xF] = registers[GetXRegister(opcode)] >> 7;
    registers[GetXRegister(opcode)] <<= 1;
}

void DumpRegisters(){
    for(int i = 0; i < REGISTER_COUNT; i++){
        printf("V[%x]:%x|%d\n", i, registers[i], registers[i]);
    }
}
