/* ---------------------------------------------------------------------------
**
** VoxelGrid.h
**
** Represents a basic voxel grind and does shit on it.
**
** Author: Zacharia Crumley
** -------------------------------------------------------------------------*/

#ifndef _VOXEL_GRID_H
#define _VOXEL_GRID_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <math.h>
#include <OGRE/Ogre.h>

#include "octree/octree.h"
#include "OgreDisplay.h"

#include "MeshExtractor.h"
#include "TriangleMesh.h"

//this is for what the octree stores. If this is defined it uses the default thing (unsigned int)
//otherwise it uses whatever is defined in the else of the macro block
//#define DEFAULT_CONTENTS

#ifdef DEFAULT_CONTENTS
//standard definition
#define OCTREE_TYPE unsigned int
#define EMPTY_VAL 0x00000000
#define OCCUPIED_VAL 0xffffffff

#else
//experimental types for trying to get Duncan's marching cubes stuff to work
#define OCTREE_TYPE float
#define EMPTY_VAL 0.0
#define OCCUPIED_VAL 1.0

#endif

//a macro for easily changing the octree's definition
#define OCTREE_DEF Octree<OCTREE_TYPE, 32>

//this is for an optimization when displaying the voxels.
#define HIDE_VOXELS

class VoxelGrid
{
	int gridSize;
	unsigned int size;
	OCTREE_DEF* grid;
	OgreDisplay* display;
	
 public:
	VoxelGrid();

	VoxelGrid(int size);

	~VoxelGrid();
	
	void makeVoxelGrid(int size);

	void displayVoxelGrid();

	void createFromFile(std::string file);

	void setDisplay(OgreDisplay* d);

	void updateDisplay();

	void polygonize();

	int getSize();

	OCTREE_TYPE getValue(int x, int y, int z);

	void makeCircle(Ogre::Vector3 pos, int radius, bool add = true);

	void makeEllipsoid(Ogre::Vector3 pos = Ogre::Vector3(0,0,0), Ogre::Vector3 extents = Ogre::Vector3(1,1,1), Ogre::Matrix3 orientation = Ogre::Matrix3(1,0,0,0,1,0,0,0,1), bool add = true);

	void makeRectangle(Ogre::Vector3 pos = Ogre::Vector3(0,0,0), Ogre::Vector3 extents = Ogre::Vector3(1,1,1), Ogre::Matrix3 orientation = Ogre::Matrix3(1,0,0,0,1,0,0,0,1), bool add = true);

	void makeCylinder(Ogre::Vector3 pos = Ogre::Vector3(0,0,0), Ogre::Vector3 extents = Ogre::Vector3(1,1,1), Ogre::Matrix3 orientation = Ogre::Matrix3(1,0,0,0,1,0,0,0,1), bool add = true); 
};

#endif
