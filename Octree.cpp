/*
  Octree.cpp
 */

#include "Octree.h"

//===================Octree Method implementations=========================

//default constructor. makes octree of size 256
Octree::Octree()
{
	this->createOctree(256);
}


//parameterized constructor, makes octree of size equal to argument's value
Octree::Octree(int in_size)
{
	this->createOctree(in_size);
}


//this method does the creation legwork of the class. It's what the
//constructors call to actually do the creation stuff.
void Octree::createOctree(int in_size)
{
	//set the size
	this->setSize(in_size);

	//make a default NodeInformation Object to initialize the octree to (empty)
	NodeInformation temp_info;
	
	root = new OctreeNode(this->getSize(), temp_info);
}


//destructor. deletes the octree.
Octree::~Octree()
{
	//if the octree doesn't actually need any destroying, do nothing
	if (root == NULL)
	{
		this->size = 0;
		return;
	}
	//if it does need destroy, do the stuff.
	else
	{
		delete this->root;
		this->root = NULL;
		this->size = 0;
	}
}


void Octree::setSize(int in)
{
	//check the size is a valid value, both positive and a power of 2
	assert(in > 0);
	assert( ((in - 1) & in) == 0 );

	//check that the tree is empty.
	assert(root == NULL);
	
	//change the size of the octree
	this->size = in;
}


int Octree::getSize()
{
	return this->size;
}


NodeInformation Octree::at(int x, int y, int z)
{
	
}


void Octree::set(int x, int y, int z, NodeInformation value)
{
	
}


void Octree::setRange(int x1, int y1, int z1, int x2, int y2, int z2, NodeInformation value)
{
	
}



void Octree::erase(int x, int y, int z)
{
	
}


void Octree::eraseRange(int x1, int y1, int z1, int x2, int y2, int z2)
{
	
}


//====================OctreeNode method implementations=========================


OctreeNode::OctreeNode()
{
	NodeInformation temp;
	this->createNode(1, temp);
}


OctreeNode::~OctreeNode()
{
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			for (int k = 0; k < 2; ++k)
			{
				delete this->children[i][j][k];
			}
}


OctreeNode::OctreeNode(int in_maxSize, NodeInformation in_info)
{
	this->createNode(in_maxSize, in_info);
}


void OctreeNode::createNode(int in_maxSize, NodeInformation in_info)
{
	this->nodeSize = in_maxSize;
	this->info = in_info;
	
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			for (int k = 0; k < 2; ++k)
			{
				this->children[i][j][k] = NULL;
			}
}


void OctreeNode::setNodeSize(int in)
{
	//check the size is a valid value, both positive and a power of 2
	assert(in > 0);
	assert( ((in - 1) & in) == 0 );

	this->nodeSize = in;
}


int OctreeNode::getNodeSize()
{
	return this->nodeSize;
}


//=================NodeInformation methods========================

NodeInformation::NodeInformation()
{
	solid = SPACE_EMPTY;
}

NodeInformation::~NodeInformation()
{

}
