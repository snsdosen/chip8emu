#Executable name
OBJ_NAME = chip8emu

#Compiler
CC = g++

LIBRARY_PATHS = -L/Library/Frameworks/SDL2.framework/ #-L/Library/Frameworks/SDL2_image.framework/
INCLUDE_PATHS = -I/Library/Frameworks/SDL2.framework/Headers -I./include/ #-I/Library/Frameworks/SDL2_image.framework/Headers

OBJS = ./src/cpu.cpp ./src/font.cpp ./src/gpu.cpp ./src/keyboard.cpp ./src/ram.cpp ./src/spu.cpp main.cpp

COMPILER_FLAGS =

#Libraries
LINKER_FLAGS = -F/Library/Frameworks -framework SDL2 #-F/Library/Frameworks -framework SDL2_image

all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
