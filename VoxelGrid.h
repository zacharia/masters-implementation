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


#include "Octree.h"
#include "OgreDisplay.h"
#include "MeshGenerator.h"

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
		int priority;
		std::set<std::string> tags;
	};

	struct ShapeComparisionObject
	{
		bool operator()(const Shape a, const Shape b) const
		{
			return a.priority < b.priority;
		}
	};
	
	int gridSize;
	unsigned int size;
	Octree* grid;
	OgreDisplay* display;
	double object_addition_granularity;	
	bool verbose;
	bool useMarchingCubes;
	bool usePriorities;

	//these store the smallest and largest corners of the bounding box of the contents of the voxel grid
	Ogre::Vector3 bounding_box_min;
	Ogre::Vector3 bounding_box_max;

	//a vector that contains the shapes that are read in from file.
	std::vector<Shape> shapes;

	//the path to the file containing the automata rule set for doing surface detail
	std::string automata_rules_file;

	//the name of the method to call in the python automata grammar. if this is "", then "main" is used.
	std::string automata_rules_method;

	int automata_num_iterations;
	int automata_neighbourhood_size;

 public:
	VoxelGrid();

	VoxelGrid(int size);

	~VoxelGrid();
	
	void makeVoxelGrid(int size);

	std::string toString();

	void outputString(std::string str, std::string path);

	void readFromFile(std::string file);

	void getBoundingBoxes();

	void scaleShapes();

	void createShapes();

	void setDisplay(OgreDisplay* d);

	void updateDisplay();

	void doSurfaceDetail();

	void polygonize();

	int getSize();

	Ogre::Vector3 getBoundingBoxMinCorner();

	Ogre::Vector3 getBoundingBoxMaxCorner();

	SPACE_TYPE getValue(int x, int y, int z);

	void makeCircle(Ogre::Vector3 pos, int radius, bool add = true, std::set<std::string> in_tags = std::set<std::string>());

	void makeEllipsoid(Ogre::Vector3 pos = Ogre::Vector3(0,0,0), Ogre::Vector3 extents = Ogre::Vector3(1,1,1), Ogre::Matrix3 orientation = Ogre::Matrix3(1,0,0,0,1,0,0,0,1), bool add = true, std::set<std::string> in_tags = std::set<std::string>());

	void makeRectangle(Ogre::Vector3 pos = Ogre::Vector3(0,0,0), Ogre::Vector3 extents = Ogre::Vector3(1,1,1), Ogre::Matrix3 orientation = Ogre::Matrix3(1,0,0,0,1,0,0,0,1), bool add = true, std::set<std::string> in_tags = std::set<std::string>());

	void makeCylinder(Ogre::Vector3 pos = Ogre::Vector3(0,0,0), Ogre::Vector3 extents = Ogre::Vector3(1,1,1), Ogre::Matrix3 orientation = Ogre::Matrix3(1,0,0,0,1,0,0,0,1), bool add = true, std::set<std::string> in_tags = std::set<std::string>());

	void setAdditionGranularity(double g);

	void setChunkSize(unsigned int s);

	void setVerbose(bool v);

	void setUseCubes(bool v);

	void setUsePriorities(bool v);

	void setAutomataRuleSet(std::string file);

	void setAutomataRuleMethod(std::string in);

	void setAutomataIterations(int in);

	void setAutomataNeighbourhoodSize(int in);
};

#endif
