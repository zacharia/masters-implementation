/*
  Name: Octree.h
  
  Description:
  My own octree class.
  
*/

#ifndef _OCTREE_CLASS_H
#define _OCTREE_CLASS_H

//standard library inclusions
#include <cstddef>
#include <cassert>
#include <OGRE/OgreVector3.h>

//project inclusions
#include "Utility.h"


enum SpaceState
{
	SPACE_EMPTY = 0,
	SPACE_SOLID = 1
};


class NodeInformation
{
public:
	//stores the physical state of the node, i.e. empty or solid
	char solid;

	//default constructor - makes a totally empty node with nothing in it and not tags
	NodeInformation();

	NodeInformation(int in_solid);

	~NodeInformation();

	bool operator==(NodeInformation& in);

	bool operator!=(NodeInformation& in);
};


class OctreeNode
{
private:
	
public:
        //the node's children
	OctreeNode* children[2][2][2];
	
	//more stuff
	NodeInformation info;

	//the max size in one dimension that this node can have.
	int nodeSize;

	OctreeNode();

	OctreeNode(int in_maxSize);

	OctreeNode(int in_maxSize, NodeInformation in_info);

	void createNode(int in_maxSize, NodeInformation in_info);

	~OctreeNode();

	void setNodeSize(int in);

	int getNodeSize();
	
	NodeInformation at(int x, int y, int z, int currSize);

	void set(int x, int y, int z, NodeInformation value, int currSize);

	void setRange(Ogre::Vector3 lower, Ogre::Vector3 upper, Ogre::Vector3 node_center, NodeInformation value);

	void erase(int x, int y, int z, int currSize);

	void eraseRange(Ogre::Vector3 lower, Ogre::Vector3 upper, Ogre::Vector3 node_center);

	void optimizeNode();

	std::string printNode(int depth = 0);
};


class Octree
{
private:
	
	OctreeNode* root;
	unsigned int size;

	//NB: null does not necessarily imply that a node is empty. The NodeInformation is used for that.
        
public:
	//these variables store information about auto-optimizing the tree after every N calls of the set method.
	bool auto_optimize_on;
	int auto_optimize_interval;
	int auto_optimize_counter;
	
	Octree();

	Octree(int size);

	void createOctree(int size);

	~Octree();

	void setSize(int in);

	int getSize();

	NodeInformation at(int x, int y, int z);

	void set(int x, int y, int z, NodeInformation value);

	void setRange(Ogre::Vector3 lower, Ogre::Vector3 upper, NodeInformation value);

	void erase(int x, int y, int z);

	void eraseRange(Ogre::Vector3 lower, Ogre::Vector3 upper);

	void optimizeTree();

	std::string printTree();
};

#endif
