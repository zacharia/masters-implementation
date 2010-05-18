# Makefile for Voxel prototype

CC=g++
WFLAGS=-Wall -Wextra -g

all: main.cpp Utility VoxelGrid InputManager OgreDisplay DerivationTree Interpreter
	$(CC) $(WFLAGS) -o prototype main.cpp VoxelGrid.o InputManager.o OgreDisplay.o Interpreter.o DerivationTree.o Utility.o -lOgreMain -lOIS

VoxelGrid: VoxelGrid.cpp VoxelGrid.h
	$(CC) $(WFLAGS) -c -o VoxelGrid.o VoxelGrid.cpp

InputManager: InputManager.h InputManager.cpp
	$(CC) $(WFLAGS) -c -o InputManager.o InputManager.cpp

OgreDisplay: OgreDisplay.h OgreDisplay.cpp
	$(CC) $(WFLAGS) -c -o OgreDisplay.o OgreDisplay.cpp

Interpreter: Interpreter.cpp Interpreter.h FlexParser
	$(CC) $(WFLAGS) -c -o Interpreter.o Interpreter.cpp

FlexParser: parser.l
	flex parser.l

DerivationTree: DerivationTree.cpp DerivationTree.h
	$(CC) $(WFLAGS) -c -o DerivationTree.o DerivationTree.cpp

Utility: Utility.cpp Utility.h
	$(CC) $(WFLAGS) -c -o Utility.o Utility.cpp

clean:
	rm *.o prototype lex.yy.c
