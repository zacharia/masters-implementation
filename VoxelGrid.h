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
#include <cstdlib>
#include <math.h>
#include <OGRE/Ogre.h>

class VoxelGrid
{
	int gridX, gridY, gridZ;
	char*** grid;
	
 public:
	VoxelGrid();

	VoxelGrid(int x, int y, int z);

	~VoxelGrid();
	
	void makeVoxelGrid(int x, int y, int z);

	void initializeVoxelGrid(char val);

	void displayVoxelGrid();

	int getXSize();

	int getYSize();

	int getZSize();

	char getValue(int x, int y, int z);

	//void makeCircle();

	void makeEllipsoid(Ogre::Vector3 pos, Ogre::Vector3 dims, bool add = true);

	void makeRectangle(Ogre::Vector3 pos, Ogre::Vector3 dims, bool add = true);

	void makeCylinder(Ogre::Vector3 pos, Ogre::Vector3 dims, char orientation = 'y', bool add = true); 
};

#endif
