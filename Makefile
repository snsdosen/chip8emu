#Executable name
OBJ_NAME = chip8emu

#Compiler
CC = g++

LIBRARY_PATHS = -L/Library/Frameworks/SDL2.framework/
INCLUDE_PATHS = -I/Library/Frameworks/SDL2.framework/Headers -I./include/

OBJS = ./src/cpu.cpp ./src/font.cpp ./src/gpu.cpp ./src/keyboard.cpp ./src/ram.cpp ./src/spu.cpp main.cpp

COMPILER_FLAGS =

#Libraries
LINKER_FLAGS = -framework SDL2

all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
