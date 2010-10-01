# Makefile for Voxel prototype

CC=g++
WFLAGS=-Wall -Wextra -g -O2

all: main.cpp Utility.o VoxelGrid.o InputManager.o OgreDisplay.o DerivationTree.o GrammarRule.o Interpreter.o
	$(CC) $(WFLAGS) -o prototype main.cpp VoxelGrid.o InputManager.o OgreDisplay.o GrammarRule.o Interpreter.o DerivationTree.o Utility.o -lOgreMain -lOIS

VoxelGrid.o: VoxelGrid.cpp VoxelGrid.h
	$(CC) $(WFLAGS) -c -o VoxelGrid.o VoxelGrid.cpp

InputManager.o: InputManager.h InputManager.cpp
	$(CC) $(WFLAGS) -c -o InputManager.o InputManager.cpp

OgreDisplay.o: OgreDisplay.h OgreDisplay.cpp
	$(CC) $(WFLAGS) -c -o OgreDisplay.o OgreDisplay.cpp

Interpreter.o: Interpreter.cpp Interpreter.h lex.yy.c
	$(CC) $(WFLAGS) -c -o Interpreter.o Interpreter.cpp

GrammarRule.o: GrammarRule.cpp GrammarRule.h
	$(CC) $(WFLAGS) -c -o GrammarRule.o GrammarRule.cpp

lex.yy.c: parser.l
	flex parser.l

DerivationTree.o: DerivationTree.cpp DerivationTree.h
	$(CC) $(WFLAGS) -c -o DerivationTree.o DerivationTree.cpp

Utility.o: Utility.cpp Utility.h
	$(CC) $(WFLAGS) -c -o Utility.o Utility.cpp

clean:
	rm *.o prototype lex.yy.c
