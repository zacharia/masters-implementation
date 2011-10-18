# Makefile for Voxel prototype

CC=g++
WFLAGS=-Wall -Wextra -g3 -O3
PROGRAM_NAME=voxel_generator

$(PROGRAM_NAME): Utility.o Octree.o MeshGenerator.o VoxelGrid.o InputManager.o OgreDisplay.o main.o
	$(CC) $(WFLAGS) -o $(PROGRAM_NAME) main.o Octree.o MeshGenerator.o VoxelGrid.o InputManager.o OgreDisplay.o Utility.o -lOgreMain -lOIS -lpython2.7 -lrt

main.o: main.cpp
	$(CC) $(WFLAGS) -c -o main.o main.cpp

VoxelGrid.o: VoxelGrid.cpp VoxelGrid.h
	$(CC) $(WFLAGS) -c -o VoxelGrid.o VoxelGrid.cpp

InputManager.o: InputManager.h InputManager.cpp
	$(CC) $(WFLAGS) -c -o InputManager.o InputManager.cpp

OgreDisplay.o: OgreDisplay.h OgreDisplay.cpp
	$(CC) $(WFLAGS) -c -o OgreDisplay.o OgreDisplay.cpp

Utility.o: Utility.cpp Utility.h
	$(CC) $(WFLAGS) -c -o Utility.o Utility.cpp

Octree.o: Octree.cpp Octree.h
	$(CC) $(WFLAGS) -c -o Octree.o Octree.cpp

MeshGenerator.o: MeshGenerator.cpp MeshGenerator.h
	$(CC) $(WFLAGS) -c -o MeshGenerator.o MeshGenerator.cpp

clean:
	rm *.o $(PROGRAM_NAME)
