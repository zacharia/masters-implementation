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
	
	root = new OctreeNode(this->getSize(), temp_info, NULL);
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
	assert(x >= 0 && x < this->size);
	assert(y >= 0 && y < this->size);
	assert(z >= 0 && z < this->size);

	//TODO: code an iterative way of doing this method, for
	//efficiency. This current way is because it's more
	//understandable, but the function calls make it less
	//efficient.
	return root->at(x,y,z, this->size);
}


void Octree::set(int x, int y, int z, NodeInformation value)
{
	assert(x >= 0 && x < this->size);
	assert(y >= 0 && y < this->size);
	assert(z >= 0 && z < this->size);

	//TODO: iterative way of doing this, instead of recursive.
	root->set(x,y,z, value, this->size);
}


void Octree::setRange(int x1, int y1, int z1, int x2, int y2, int z2, NodeInformation value)
{
	
}


void Octree::erase(int x, int y, int z)
{
	assert(x >= 0 && x < this->size);
	assert(y >= 0 && y < this->size);
	assert(z >= 0 && z < this->size);

	//TODO: iterative way of doing this.
	root->erase(x,y,z, this->size);
}


void Octree::eraseRange(int x1, int y1, int z1, int x2, int y2, int z2)
{
	
}


//====================OctreeNode method implementations=========================


OctreeNode::OctreeNode()
{
	NodeInformation temp;
	this->createNode(1, temp, NULL);
}


OctreeNode::~OctreeNode()
{
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			for (int k = 0; k < 2; ++k)
			{
				if (this->children[i][j][k] != NULL)
					delete this->children[i][j][k];
			}
}


OctreeNode::OctreeNode(int in_maxSize, OctreeNode* in_parent)
{
	NodeInformation temp;
	this->createNode(in_maxSize, temp, in_parent);
}


OctreeNode::OctreeNode(int in_maxSize, NodeInformation in_info, OctreeNode* in_parent)
{
	this->createNode(in_maxSize, in_info, in_parent);
}


void OctreeNode::createNode(int in_maxSize, NodeInformation in_info, OctreeNode* in_parent)
{
	this->nodeSize = in_maxSize;
	this->info = in_info;
	this->parent = in_parent;
	
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


//Recursive method for retrieving a value from the octree.
//currSize is used for choosing the node to recurse to.
NodeInformation OctreeNode::at(int x, int y, int z, int currSize)
{
	//if we're at a node with no children
	if ((this->children[0][0][0] == NULL) &&
	      (this->children[0][0][1] == NULL) &&
	      (this->children[0][1][0] == NULL) &&
	      (this->children[0][1][1] == NULL) &&
	      (this->children[1][0][0] == NULL) &&
	      (this->children[1][0][1] == NULL) &&
	      (this->children[1][1][0] == NULL) &&
	      (this->children[1][1][1] == NULL))
	{
		//return the current node's NodeInformation
		return this->info;
	}
	//otherwise if there are children
	else
	{
		//recurse to the correct one.
		//I got this code for selecting which child to go to from the octree class I was using. hope it works.
		int newSize = currSize / 2;
		return this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)]->at(x,y,z, newSize);
	}
}


//A recursive method for setting a value. It avoids creating
//unnecessary nodes, where possible.
void OctreeNode::set(int x, int y, int z, NodeInformation value, int currSize)
{
	//if we're at a leaf node
	if (currSize == 1)
	{
		//then set it to the value
		this->info = value;
	}
	//if the current node has no children and is already set to the given value
	else if ((this->info == value) &&
		 (this->children[0][0][0] == NULL) &&
		 (this->children[0][0][1] == NULL) &&
		 (this->children[0][1][0] == NULL) &&
		 (this->children[0][1][1] == NULL) &&
		 (this->children[1][0][0] == NULL) &&
		 (this->children[1][0][1] == NULL) &&
		 (this->children[1][1][0] == NULL) &&
		 (this->children[1][1][1] == NULL) )
	{
		//then we don't need to do anything, since the value is already set.
		return;
	}
	//otherwise, we need to recurse
	else
	{
		//determine which child the element to set lies in.
		int newSize = currSize / 2;
		OctreeNode* nextNode = this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)];

		//if that child node is currently empty.
		if (nextNode == NULL)
		{
			//then instantiate it.
			this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)] = new OctreeNode(newSize, this->info, this);
			this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)]->info = this->info;
			this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)]->set(x,y,z, value, newSize);
		}
		else
		{
			//otherwise, recurse on it.
			nextNode->set(x,y,z, value, newSize);
		}
	}
}

void OctreeNode::setRange(int x1, int y1, int z1, int x2, int y2, int z2, NodeInformation value)
{
	
}


//A recursive method for deleting a node. This means removing it from
//the tree, not just changing it's NodeInformation to empty.
void OctreeNode::erase(int x, int y, int z, int currSize)
{
        //if we're at a node with no children
	if ((this->children[0][0][0] == NULL) &&
	    (this->children[0][0][1] == NULL) &&
	    (this->children[0][1][0] == NULL) &&
	    (this->children[0][1][1] == NULL) &&
	    (this->children[1][0][0] == NULL) &&
	    (this->children[1][0][1] == NULL) &&
	    (this->children[1][1][0] == NULL) &&
	    (this->children[1][1][1] == NULL))
	{
		//then the value does not need to be erased.
		return;
	}
	else
	{
		//determine which child the element to erase lies in.
		int newSize = currSize / 2;
		OctreeNode* nextNode = this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)];

		//if the child to delete is a leaf node
		if (newSize == 1)
		{
			//then delete it, if it isn't already NULL
			if (nextNode != NULL)
			{
				delete this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)];
				this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)] = NULL;
			}
		}
		//otherwise, recurse on it
		else
		{
			this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)]->erase(x,y,z, newSize);
		}
	}
}


void OctreeNode::eraseRange(int x1, int y1, int z1, int x2, int y2, int z2)
{
	
}


//This method does a postorder walk of the tree, removing redundant
//nodes. If all children of a node are non-NULL and they all have the
//same NodeInformation, they can be deleted and the parent's
//NodeInformation changed to theirs.
void OctreeNode::removeRedundantCells()
{
	//first recurse on all children.
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			for (int k = 0; k < 2; ++k)
			{
				this->children[i][j][k]->removeRedundantCells();
			}
	
        //if we're at a node with no children
	if ((this->children[0][0][0] == NULL) &&
	    (this->children[0][0][1] == NULL) &&
	    (this->children[0][1][0] == NULL) &&
	    (this->children[0][1][1] == NULL) &&
	    (this->children[1][0][0] == NULL) &&
	    (this->children[1][0][1] == NULL) &&
	    (this->children[1][1][0] == NULL) &&
	    (this->children[1][1][1] == NULL))
	{
		//then we can't actually do anything with this node.
		return;
	}
	else
	{
		//if all the children are non-NULL
		if ((this->children[0][0][0] != NULL) &&
		    (this->children[0][0][1] != NULL) &&
		    (this->children[0][1][0] != NULL) &&
		    (this->children[0][1][1] != NULL) &&
		    (this->children[1][0][0] != NULL) &&
		    (this->children[1][0][1] != NULL) &&
		    (this->children[1][1][0] != NULL) &&
		    (this->children[1][1][1] != NULL))
		{
			//then check if they all have the same info.
			NodeInformation comp_value;
			bool got_val = false;
			bool can_coalesce = true;

			for (int i = 0; i < 2; ++i)
				for (int j = 0; j < 2; ++j)
					for (int k = 0; k < 2; ++k)
					{
						if ( (!got_val) && (this->children[i][j][k] != NULL) )
						{
							comp_value = this->children[i][j][k]->info;
							got_val = true;
						}
						//if they don't all have the same info
						if ((this->children[i][j][k] != NULL) && (this->children[i][j][k]->info != comp_value))
						{
							//return, since this node can't be optimized.
							can_coalesce = false;
							return;
						}
					}

			//if the children of the node can be optimized away
			if (can_coalesce)
			{
				//then set this node's value to the children's
				this->info = comp_value;

				//and remove the children.
				for (int i = 0; i < 2; ++i)
					for (int j = 0; j < 2; ++j)
						for (int k = 0; k < 2; ++k)
						{
							delete this->children[i][j][k];
							this->children[i][j][k] = NULL;
						}
			}
		}
	}
}


//=================NodeInformation methods========================

NodeInformation::NodeInformation()
{
	solid = SPACE_EMPTY;
}

NodeInformation::~NodeInformation()
{

}

bool NodeInformation::operator==(NodeInformation& in)
{
	return ( (this->solid == in.solid) );
}

bool NodeInformation::operator!=(NodeInformation& in)
{
	return !( (this->solid == in.solid) );
}
