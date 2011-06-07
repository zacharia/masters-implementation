/*
  Octree.cpp
 */

#include "Octree.h"

//===================Octree Method implementations=========================

//default constructor. makes octree of size 256
Octree::Octree()
{
	root = NULL;
	this->createOctree(256);
}


//parameterized constructor, makes octree of size equal to argument's value
Octree::Octree(int in_size)
{
	root = NULL;
	this->createOctree(in_size);
}


//this method does the creation legwork of the class. It's what the
//constructors call to actually do the creation stuff.
void Octree::createOctree(int in_size)
{
	//destroy any existing octree before we make a new one
	if (root != NULL)
	{
		delete root;
		this->root = NULL;	
	}
		
	//set the size
	this->setSize(in_size);

	//make a default VoxelInformation Object to initialize the octree to (empty)
	VoxelInformation temp_info;

	//create the root node.
	this->root = new OctreeNode(this->getSize(), temp_info);

	//set up the default auto_optimize values.
	this->auto_optimize_on = true;
	this->auto_optimize_interval = in_size * in_size;
	this->auto_optimize_counter = 0;
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


VoxelInformation Octree::at(int x, int y, int z)
{
	// I commented these out in favour of just returning an empty value when asking for something outside the octree
	// assert(x >= 0 && x < this->size);
	// assert(y >= 0 && y < this->size);
	// assert(z >= 0 && z < this->size);

	if ( (x < 0) || (x > this->size) || (y < 0) || (y > this->size) || (z < 0) || (z > this->size) )
	{
		return VoxelInformation(SPACE_EMPTY);
	}

	//TODO: code an iterative way of doing this method, for
	//efficiency. This current way is because it's more
	//understandable, but the function calls make it less
	//efficient.
	return root->at(x,y,z, this->size);
}


void Octree::set(int x, int y, int z, VoxelInformation value)
{
	assert(x >= 0 && x < this->size);
	assert(y >= 0 && y < this->size);
	assert(z >= 0 && z < this->size);

	//TODO: iterative way of doing this, instead of recursive.
	root->set(x,y,z, value, this->size);

	this->auto_optimize_counter++;
	if ((this->auto_optimize_on) && (this->auto_optimize_counter >= this->auto_optimize_interval))
	{
		this->optimizeTree();
		this->auto_optimize_counter = 0;
	}
}


//This method sets all values of an axis-aligned rectangle in the
//octree, smallest and largest corners given as args, to the value
//given as an arg.
void Octree::setRange(Ogre::Vector3 lower, Ogre::Vector3 upper, VoxelInformation value)
{
	root->setRange(lower, upper, Ogre::Vector3(this->size / 2), value);
}


//This method erases a value from the tree.
void Octree::erase(int x, int y, int z)
{
	assert(x >= 0 && x < this->size);
	assert(y >= 0 && y < this->size);
	assert(z >= 0 && z < this->size);

	//TODO: iterative way of doing this.
	root->erase(x,y,z, this->size);

	this->auto_optimize_counter++;
	if ((this->auto_optimize_on) && (this->auto_optimize_counter >= this->auto_optimize_interval))
	{
		this->optimizeTree();
		this->auto_optimize_counter = 0;
	}
}


//This method erases all values contained in an axis-aligned rectangle
void Octree::eraseRange(Ogre::Vector3 lower, Ogre::Vector3 upper)
{
	root->eraseRange(lower, upper, Ogre::Vector3(this->size / 2));
}


//this method optimizes the tree of unnecessary nodes.
void Octree::optimizeTree()
{
	root->optimizeNode();
}


//this method returns a string representation of the tree.
std::string Octree::printTree()
{
	return root->printNode();
}


//====================OctreeNode method implementations=========================


//default constructor.
OctreeNode::OctreeNode()
{
	VoxelInformation temp;
	this->createNode(1, temp);
}


//recursive destructor
OctreeNode::~OctreeNode()
{
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			for (int k = 0; k < 2; ++k)
			{
				if (this->children[i][j][k] != NULL)
				{
					delete this->children[i][j][k];
					this->children[i][j][k] = NULL;
				}
			}
}


OctreeNode::OctreeNode(int in_maxSize)
{
	VoxelInformation temp;
	this->createNode(in_maxSize, temp);
}


OctreeNode::OctreeNode(int in_maxSize, VoxelInformation in_info)
{
	this->createNode(in_maxSize, in_info);
}


//this is the method that's actually used by the constructors to make
//the node.
void OctreeNode::createNode(int in_maxSize, VoxelInformation in_info)
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


//get and set methods for nodeSize. can probably ignore these methods
//(just use ->nodeSize and ->nodeSize =) for performance gains.
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
VoxelInformation OctreeNode::at(int x, int y, int z, int currSize)
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
		//return the current node's VoxelInformation
		return this->info;
	}
	//otherwise if there are children
	else
	{
		//I got this code for selecting which child to go to from the octree class I was using. hope it works.
		int newSize = currSize / 2;
		//recurse on the correct child node if it's non-null, else return the current node's value
		if (this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)] == NULL)
		{
			return this->info;
		}
		else
		{
			return this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)]->at(x,y,z, newSize);	
		}		
	}
}


//A recursive method for setting a value. It avoids creating
//unnecessary nodes, where possible.
void OctreeNode::set(int x, int y, int z, VoxelInformation value, int currSize)
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
			this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)] = new OctreeNode(newSize, this->info);
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


//recursive method for setting an axis-aligned rectangle of values in the octree.
void OctreeNode::setRange(Ogre::Vector3 lower, Ogre::Vector3 upper, Ogre::Vector3 node_center, VoxelInformation value)
{
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			for (int k = 0; k < 2; ++k)
			{
				// OctreeNode* curr_child = this->children[i][j][k];

				// Ogre::Vector3 diff;
				
				
				// if ()
				// {
					
				// }
				// else
				// {
					
				// }
			}	
}


//A recursive method for deleting a node. This means removing it from
//the tree, not just changing it's VoxelInformation to empty.
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


//recursive method for erasing an axis-aligned rectangle of nodes from the tree.
void OctreeNode::eraseRange(Ogre::Vector3 lower, Ogre::Vector3 upper, Ogre::Vector3 node_center)
{
	
}


//This method does a postorder walk of the tree, removing redundant
//nodes. If all children of a node are non-NULL and they all have the
//same VoxelInformation, they can be deleted and the parent's
//VoxelInformation changed to theirs.
void OctreeNode::optimizeNode()
{
	//first recurse on all children.
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			for (int k = 0; k < 2; ++k)
			{
				if (this->children[i][j][k] != NULL)
					this->children[i][j][k]->optimizeNode();
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
			//NB: we know that all children are non-NULL
		       
			//comp_value stores info of the first child. if any of the other children have a different ->info, we can't coalesce
			VoxelInformation comp_value = this->children[0][0][0]->info;
			
			bool can_coalesce = true;

			//loop through all children
			for (int i = 0; i < 2; ++i)
				for (int j = 0; j < 2; ++j)
					for (int k = 0; k < 2; ++k)
					{
						//if any of the children of the child currently being examined are non-NULL, we can't optimize away this node.
						if ((this->children[i][j][k]->children[0][0][0] != NULL) ||
						    (this->children[i][j][k]->children[0][0][1] != NULL) ||
						    (this->children[i][j][k]->children[0][1][0] != NULL) ||
						    (this->children[i][j][k]->children[0][1][1] != NULL) ||
						    (this->children[i][j][k]->children[1][0][0] != NULL) ||
						    (this->children[i][j][k]->children[1][0][1] != NULL) ||
						    (this->children[i][j][k]->children[1][1][0] != NULL) ||
						    (this->children[i][j][k]->children[1][1][1] != NULL))
						{
							//return, since this node can't be optimized.
							can_coalesce = false;
							break;
						}
						//if they don't all have the same info
						if (this->children[i][j][k]->info != comp_value)
						{
							//return, since this node can't be optimized.
							can_coalesce = false;
							break;
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
		else
		{
			//loop through all the children.
			for (int i = 0; i < 2; ++i)
				for (int j = 0; j < 2; ++j)
					for (int k = 0; k < 2; ++k)
					{
						if (this->children[i][j][k] != NULL)
						{
							//this checks if all children of the child being
							//examined are null. If they are, then the current
							//child can be made NULL.
							bool all_children_null = true;
							for (int x = 0; x < 2; ++x)
								for (int y = 0; y < 2; ++y)
									for (int z = 0; z < 2; ++z)
									{
										if (this->children[i][j][k]->children[x][y][z] != NULL)
										{
											all_children_null = false;
										}
									}
							if ((all_children_null) && (this->children[i][j][k]->info.solid == this->info.solid))
							{
								delete this->children[i][j][k];
								this->children[i][j][k] = NULL;
							}	
						}
						
					}
		}
	}
}


//this method returns a string representation of the current node and
//it's children. depth is used in formatting the string correctly.
std::string OctreeNode::printNode(int depth)
{
	std::string ret = "";

	//add in the correct number of spaces
	for (int i = 0; i < depth; ++i)
	{
		ret += "  ";
	}

	//append this node's information
	ret += "Node: ";
	ret += Utility::numToString(this->nodeSize);
	ret += " ";
	ret += Utility::numToString(this->info.solid);
	ret += " ";
	for (std::set<std::string>::iterator i = this->info.tags.begin(); i != this->info.tags.end(); i++)
	{
		ret = ret + *i + " ";
	}
	ret += "\n";

	//if this is not a leaf node
	if (this->nodeSize > 1)
	{
		//then recurse on any non-NULL children and add their string representation to the return string.
		for (int i = 0; i < 2; ++i)
			for (int j = 0; j < 2; ++j)
				for (int k = 0; k < 2; ++k)
				{
					if (this->children[i][j][k] == NULL)
					{
						for (int i = 0; i < depth+1; ++i)
						{
							ret += "  ";
						}
						ret += "NULL\n";
					}
					else
					{
						ret += this->children[i][j][k]->printNode(depth + 1);
					}
				}	
	}

	return ret;
}


//=================VoxelInformation methods========================

VoxelInformation::VoxelInformation()
{
	solid = SPACE_EMPTY;
	tags = std::set<std::string>();
}


VoxelInformation::VoxelInformation(SPACE_TYPE in_solid, std::set<std::string> in_tags)
{
	solid = in_solid;
	tags = std::set<std::string>(in_tags);
}


VoxelInformation::~VoxelInformation()
{

}


bool VoxelInformation::operator==(VoxelInformation& in)
{
	return ( (this->solid == in.solid) && (this->tags == in.tags) );
}


bool VoxelInformation::operator!=(VoxelInformation& in)
{
	return !( (this->solid == in.solid) && (this->tags == in.tags) );
}
