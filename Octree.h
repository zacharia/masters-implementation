/*
  Name: Octree.h
  
  Description:
  My own octree class.
  
*/

#ifndef _OCTREE_CLASS_H
#define _OCTREE_CLASS_H

//the C/python interface for embedding python
#include <python2.7/Python.h>

//standard library inclusions
#include <cstddef>
#include <cassert>
#include <OGRE/OgreVector3.h>
#include <vector>

//project inclusions
#include "Utility.h"


//this is for what the octree stores. If this is defined it uses the default thing (unsigned int)
//otherwise it uses whatever is defined in the else of the macro block
//#define DEFAULT_CONTENTS

#ifdef DEFAULT_CONTENTS
//standard definition
#define SPACE_TYPE int
#define SPACE_EMPTY 0
#define SPACE_SOLID 1000
#define SPACE_BOUNDARY_VAL 500

#else
//experimental types for trying to get Duncan's marching cubes stuff to work
#define SPACE_TYPE float
#define SPACE_EMPTY 0.0
#define SPACE_SOLID 1.0
#define SPACE_BOUNDARY_VAL 0.5

#endif

class VoxelInformation
{
public:
	//stores the physical state of the node, i.e. empty or solid
	SPACE_TYPE solid;

	//stores the tags of the node.
	std::set<std::string> tags;

	//these store information about this node when it's aggregated.
	SPACE_TYPE aggregate_solid;
	std::set<std::string> aggregate_tags;
	Ogre::Vector3 aggregate_normal;

	//these store information about what detail should be applied to a node.
	std::set<std::string> detail_info;

	//default constructor - makes a totally empty node with nothing in it and not tags
	VoxelInformation();

	VoxelInformation(SPACE_TYPE in_solid, std::set<std::string> in_tags = std::set<std::string>());

	~VoxelInformation();

	bool operator==(VoxelInformation& in);

	bool operator!=(VoxelInformation& in);
};

//this is here to allow the OctreeNode class to have a pointer to the Octree object for checking edge nodes.
class Octree;

class OctreeNode
{
private:
	bool allChildrenNull();
	
public:
        //the node's children
	OctreeNode* children[2][2][2];
	
	//more stuff
	VoxelInformation info;

	//the max size in one dimension that this node can have.
	int nodeSize;

	OctreeNode();

	OctreeNode(const OctreeNode& copy_from_me);

	OctreeNode(int in_maxSize);

	OctreeNode(int in_maxSize, VoxelInformation in_info);

	void createNode(int in_maxSize, VoxelInformation in_info);

	~OctreeNode();

	void setNodeSize(int in);

	int getNodeSize();
	
	VoxelInformation at(int x, int y, int z, int currSize, int depth_restriction = 1);

	VoxelInformation* at_pointer(int x, int y, int z, int currSize, int depth_restriction = 1);

	void set(int x, int y, int z, VoxelInformation value, int currSize);

	void setRange(Ogre::Vector3 lower, Ogre::Vector3 upper, Ogre::Vector3 node_center, VoxelInformation value);

	void erase(int x, int y, int z, int currSize);

	void eraseRange(Ogre::Vector3 lower, Ogre::Vector3 upper, Ogre::Vector3 node_center);

	void optimizeNode();

	std::string printNode(int depth = 0);

	void makeAggregateInformation();

	std::set<Ogre::Vector3, VectorLessThanComparator> getSurfaceVoxels(Ogre::Vector3 corner, int currSize, Octree* tree, char connectivity = 26, int adjacentVoxelBorderSize = 0);

	std::set<Ogre::Vector3, VectorLessThanComparator> getAllSolidVoxels(Ogre::Vector3 corner, int currSize, Octree* tree);
};


class Octree
{
private:
	
	OctreeNode* root;
	int size;

	bool verbose;

	//NB: null does not necessarily imply that a node is empty. The VoxelInformation is used for that.
        
public:
	//these variables store information about auto-optimizing the tree after every N calls of the set method.
	bool auto_optimize_on;
	int auto_optimize_interval;
	int auto_optimize_counter;
	
	Octree();

	Octree(const Octree& copy_from_me);
	
	Octree(int size);

	void createOctree(int size);

	~Octree();

	void setSize(int in);

	int getSize();

	VoxelInformation at(int x, int y, int z, int depth_restriction = 1);

	VoxelInformation* at_pointer(int x, int y, int z, int depth_restriction = 1);

	void set(int x, int y, int z, VoxelInformation value);

	void setRange(Ogre::Vector3 lower, Ogre::Vector3 upper, VoxelInformation value);

	void erase(int x, int y, int z);

	void eraseRange(Ogre::Vector3 lower, Ogre::Vector3 upper);

	void optimizeTree();

	std::string printTree();

	void makeAggregateInformation();

	std::set<Ogre::Vector3, VectorLessThanComparator> getSurfaceVoxels(char connectivity = 26, int adjacentVoxelBorderSize = 0);

	bool isEdgeVoxel(Ogre::Vector3 pos, char connectivity);

	PyObject* convertToDict(const VoxelInformation& in);

	PyObject* convertToList(const std::set<std::string>& in);

	PyObject* convertToList(const Ogre::Vector3& in);

	void runAutomataRules(std::string rules_file, std::string rules_method, int max_iterations = -1, int neighbourhood_radius = -1);

	std::set<Ogre::Vector3, VectorLessThanComparator> getAllSolidVoxels();

	void setVerbose(bool v);
};

#endif
