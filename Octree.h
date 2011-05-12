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
// HERE

//these are used in the NodeInformation class to represent if a node is solid or empty.
#define NODE_EMPTY NodeInformation::SpaceState::SPACE_EMPTY
#define NODE_SOLID NodeInformation::SpaceState::SPACE_SOLID


class NodeInformation
{
public:
	enum SpaceState
	{
		SPACE_EMPTY = 0,
		SPACE_SOLID = 1
	};

	//stores the physical state of the node, i.e. empty or solid
	char solid;

	//default constructor - makes a totally empty node with nothing in it and not tags
	NodeInformation();

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
	OctreeNode* parent;
	
	//more stuff
	NodeInformation info;

	//the max size in one dimension that this node can have.
	int nodeSize;

	OctreeNode();

	OctreeNode(int in_maxSize, OctreeNode* in_parent);

	OctreeNode(int in_maxSize, NodeInformation in_info, OctreeNode* in_parent);

	void createNode(int in_maxSize, NodeInformation in_info, OctreeNode* in_parent);

	~OctreeNode();

	void setNodeSize(int in);

	int getNodeSize();
	
	NodeInformation at(int x, int y, int z, int currSize);

	void set(int x, int y, int z, NodeInformation value, int currSize);

	void setRange(int x1, int y1, int z1, int x2, int y2, int z2, NodeInformation value);

	void erase(int x, int y, int z, int currSize);

	void eraseRange(int x1, int y1, int z1, int x2, int y2, int z2);

	void optimizeNode();
};


class Octree
{
private:
	
	OctreeNode* root;
	unsigned int size;

	//NB: empty value is null.

public:
	Octree();

	Octree(int size);

	void createOctree(int size);

	~Octree();

	void setSize(int in);

	int getSize();

	NodeInformation at(int x, int y, int z);

	void set(int x, int y, int z, NodeInformation value);

	void setRange(int x1, int y1, int z1, int x2, int y2, int z2, NodeInformation value);

	void erase(int x, int y, int z);

	void eraseRange(int x1, int y1, int z1, int x2, int y2, int z2);
};

#endif
