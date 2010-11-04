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

//this is for what the octree stores. If this is defined it uses the default thing (unsigned int)
//otherwise it uses whatever is defined in the else of the macro block
#define DEFAULT_CONTENTS

#ifdef DEFAULT_CONTENTS
//standard definition
#define OCTREE_TYPE /*unsigned*/ int
#define EMPTY_VAL 0 //0x00000000
#define OCCUPIED_VAL 10 //0xffffffff
#define BOUNDARY_VAL 5

#else
//experimental types for trying to get Duncan's marching cubes stuff to work
#define OCTREE_TYPE float
#define EMPTY_VAL 0.0
#define OCCUPIED_VAL 1.0

#endif

//a macro for easily changing the octree's definition
#define OCTREE_DEF Octree<OCTREE_TYPE, 32>


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <math.h>
#include <OGRE/Ogre.h>

#include "octree/octree.h"
#include "OgreDisplay.h"

#include "MeshExtractor.h"
#include "TriangleMesh.h"


//this is for an optimization when displaying the voxels.
#define HIDE_VOXELS

//the biggest double I'll ever use for this, used for finding the bounding boxes
#define MAX_DOUBLE_VAL 9999999999

class VoxelGrid
{
	//this contains the information for a shape read in from file
	struct Shape
	{
		std::string type;
		bool additive;
		Ogre::Vector3 position;
		Ogre::Vector3 extents;
		Ogre::Matrix3 orientation;
	};
	
	int gridSize;
	unsigned int size;
	OCTREE_DEF* grid;
	OgreDisplay* display;
	double object_addition_granularity;
	unsigned int polygonize_chunk_size;
	bool verbose;

	//these store the smallest and largest corners of the bounding box of the contents of the voxel grid
	Ogre::Vector3 bounding_box_min;
	Ogre::Vector3 bounding_box_max;

	//a vector that contains the shapes that are read in from file.
	std::vector<Shape> shapes;
		
 public:
	VoxelGrid();

	VoxelGrid(int size);

	~VoxelGrid();
	
	void makeVoxelGrid(int size);

	std::string toString();

	std::string arrayToString(OCTREE_TYPE* array, unsigned int size);

	void outputString(std::string str, std::string path);

	void readFromFile(std::string file);

	void getBoundingBoxes();

	void scaleShapes();

	void createShapes();

	void setDisplay(OgreDisplay* d);

	void updateDisplay();

	void polygonize();

	TriangleMesh polygonizeBlock(unsigned int size, Ogre::Vector3 position = Ogre::Vector3::ZERO);

	int getSize();

	Ogre::Vector3 getBoundingBoxMinCorner();

	Ogre::Vector3 getBoundingBoxMaxCorner();

	OCTREE_TYPE getValue(int x, int y, int z);

	void makeCircle(Ogre::Vector3 pos, int radius, bool add = true);

	void makeEllipsoid(Ogre::Vector3 pos = Ogre::Vector3(0,0,0), Ogre::Vector3 extents = Ogre::Vector3(1,1,1), Ogre::Matrix3 orientation = Ogre::Matrix3(1,0,0,0,1,0,0,0,1), bool add = true);

	void makeRectangle(Ogre::Vector3 pos = Ogre::Vector3(0,0,0), Ogre::Vector3 extents = Ogre::Vector3(1,1,1), Ogre::Matrix3 orientation = Ogre::Matrix3(1,0,0,0,1,0,0,0,1), bool add = true);

	void makeCylinder(Ogre::Vector3 pos = Ogre::Vector3(0,0,0), Ogre::Vector3 extents = Ogre::Vector3(1,1,1), Ogre::Matrix3 orientation = Ogre::Matrix3(1,0,0,0,1,0,0,0,1), bool add = true);

	void setAdditionGranularity(double g);

	void setChunkSize(unsigned int s);

	void setVerbose(bool v);
};

#endif
