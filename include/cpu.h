#ifndef cpu_h
#define cpu_h

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "return_codes.h"
#include "ram.h"
#include "gpu.h"
#include "font.h"
#include "keyboard.h"
#include "spu.h"

#define REGISTER_COUNT  16
#define STACK_COUNT     16
#define HP48FLAGS_COUNT 07

#define TIMER_INTERVAL  16    //16ms aka 60Hz

void ResetCPU();
int InitCPU();
Uint32 timerCallback( Uint32 interval, void* param );
int RunOpcode(unsigned short opcode);
int RunCPU(int instructionCount, int highInstructionCount);
void UnknownOpcode(unsigned short opcode);
int IncrementProgramCounter();

//Extract info from opcode
unsigned char GetXRegister(unsigned short opcode);
unsigned char GetYRegister(unsigned short opcode);
unsigned char GetByte(unsigned short opcode);
unsigned short GetAddress(unsigned short opcode);
unsigned char GetNibble(unsigned short opcode);

//Opcode handlers
void OPC0(unsigned short opcode);
void OPC1(unsigned short opcode);
void OPC2(unsigned short opcode);
void OPC3(unsigned short opcode);
void OPC4(unsigned short opcode);
void OPC5(unsigned short opcode);
void OPC6(unsigned short opcode);
void OPC7(unsigned short opcode);
void OPC8(unsigned short opcode);
void OPC9(unsigned short opcode);
void OPCA(unsigned short opcode);
void OPCB(unsigned short opcode);
void OPCC(unsigned short opcode);
void OPCD(unsigned short opcode);
void OPCE(unsigned short opcode);
void OPCF(unsigned short opcode);

//Special functions
void DEC2BCD(unsigned char number);

void ADD(unsigned short opcode);
void SUB(unsigned short opcode);
void SHR(unsigned short opcode);
void SUBN(unsigned short opcode);
void SHL(unsigned short opcode);

void DumpRegisters();

#endif /* cpu_h */
