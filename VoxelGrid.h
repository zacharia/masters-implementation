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

//a macro for easily changing what the octree stores later on
#define OCTREE_DEF Octree<unsigned int, 32>

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

	int getSize();

	unsigned int getValue(int x, int y, int z);

	void makeCircle(Ogre::Vector3 pos, int radius, bool add = true);

	void makeEllipsoid(Ogre::Vector3 pos, Ogre::Vector3 extents, Ogre::Matrix3 orientation, bool add = true);

	void makeRectangle(Ogre::Vector3 pos, Ogre::Vector3 extents, Ogre::Matrix3 orientation, bool add = true);

	void makeCylinder(Ogre::Vector3 pos, Ogre::Vector3 extents, Ogre::Matrix3 orientation, bool add = true); 
};

#endif
