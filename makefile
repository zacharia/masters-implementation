# Makefile for Voxel prototype

CC=g++
WFLAGS=-Wall -Wextra -g -O2
PROGRAM_NAME=prototype

all: main.cpp Utility.o VoxelGrid.o InputManager.o OgreDisplay.o TriangleMesh.o MeshExtractor.o
	$(CC) $(WFLAGS) -o $(PROGRAM_NAME) main.cpp VoxelGrid.o InputManager.o OgreDisplay.o Utility.o TriangleMesh.o MeshExtractor.o -lOgreMain -lOIS

VoxelGrid.o: VoxelGrid.cpp VoxelGrid.h
	$(CC) $(WFLAGS) -c -o VoxelGrid.o VoxelGrid.cpp

InputManager.o: InputManager.h InputManager.cpp
	$(CC) $(WFLAGS) -c -o InputManager.o InputManager.cpp

OgreDisplay.o: OgreDisplay.h OgreDisplay.cpp
	$(CC) $(WFLAGS) -c -o OgreDisplay.o OgreDisplay.cpp

Utility.o: Utility.cpp Utility.h
	$(CC) $(WFLAGS) -c -o Utility.o Utility.cpp

MeshExtractor.o: MeshExtractor.h MeshExtractor.cpp
	$(CC) $(WFLAGS) -c -o MeshExtractor.o MeshExtractor.cpp

TriangleMesh.o: TriangleMesh.h TriangleMesh.cpp
	$(CC) $(WFLAGS) -c -o TriangleMesh.o TriangleMesh.cpp

clean:
	rm *.o $(PROGRAM_NAME)
