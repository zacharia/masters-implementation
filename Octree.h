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
};


class OctreeNode
{
private:
	//the node's children
	OctreeNode* children[2][2][2];

	//the max size in one dimension that this node can have.
	int nodeSize;

public:
	//more stuff
	NodeInformation info;

	OctreeNode();

	OctreeNode(int in_maxSize, NodeInformation in_info);

	void createNode(int in_maxSize, NodeInformation in_info);

	~OctreeNode();

	void setNodeSize(int in);

	int getNodeSize();
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
