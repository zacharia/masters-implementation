// ---------------------------------------------------------------------------
// DerivationTree.cpp
//
// Author: Zacharia Crumley
// ---------------------------------------------------------------------------

#include "DerivationTree.h"

//--------------------------Tree Object Methods-------------------------------

DerivationTree::DerivationTree()
{
	root = NULL;
}

DerivationTree::~DerivationTree()
{
	if (root != NULL)
	{
		delete root;
	}
}

void DerivationTree::initialize(DerivationTreeNode r)
{
	root = new DerivationTreeNode(r);
}

//----------------------------Node Object Methods-----------------------------

DerivationTreeNode::DerivationTreeNode()
{
	this->type = UNDEFINED;
	this->position = Vector3(0,0,0);
	this->extents = Vector3(0,0,0);
	this->orientation = Quaternion::IDENTITY;

	this->parent = NULL;

	children = std::vector<DerivationTreeNode>();
}

DerivationTreeNode::DerivationTreeNode(DerivationTreeNode* in, bool copyChildren)
{
	this->position = in->position;
	this->extents = in->extents;
	this->orientation = in->orientation;
	this->type = in->type;
	this->parent = NULL;
	if (copyChildren)
	{
		this->children = std::vector<DerivationTreeNode>(in->children);	
	}
}

DerivationTreeNode::~DerivationTreeNode()
{
	for (std::vector<DerivationTreeNode>::iterator i = children.begin(); i != children.end(); i++)
	{
		//do nothing?
	}

	//do nothing since there were no new's used in the creation of this?
}

void DerivationTreeNode::scaleNode(double factor)
{
	DerivationTreeNode temp = DerivationTreeNode(this); //FIXME: Should I copy the children or not?
	temp.extents *= factor;

	this->children.push_back(temp);
}

//NOTE: char is either 'x', 'y' or 'z' and indicates along which local axis to split the node
//    not everything will necessarily obey the axis thing, e.g. cylinders can only split along their x or z axes
void DerivationTreeNode::splitNode(int num, char axis)
{
	DerivationTreeNode temp;
	
	for (int i = 0; i < num; i++)
	{
		temp = DerivationTreeNode(this);

		//derive the children split nodes from the parent

		this->children.push_back(temp);
	}
}

void DerivationTreeNode::moveNode(Vector3 pos)
{
	DerivationTreeNode temp = DerivationTreeNode(this); //FIXME: Should I copy the children or not? What about just assigning the children to the node created in this method?
	temp.position += pos;

	this->children.push_back(temp);
}

void DerivationTreeNode::rotateNode(Quaternion rot)
{
	DerivationTreeNode temp = DerivationTreeNode(this); //FIXME: Should I copy the children or not?
	temp.orientation = temp.orientation * rot; //FIXME: Is this right to apply the rotation?

	this->children.push_back(temp);
}

void DerivationTreeNode::addPrimitive(int intype, Vector3 pos, Vector3 ext, Quaternion orient)
{
	DerivationTreeNode temp;
	temp.type = intype;
	temp.position = pos;
	temp. extents = ext;
	temp.orientation = orient;

	this->children.push_back(temp);
}
