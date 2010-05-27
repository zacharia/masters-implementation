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

std::string DerivationTree::displayTree()
{
	return root->displayNode(0);
}

void DerivationTree::setRoot(DerivationTreeNode* in)
{
	root = in;
}

DerivationTreeNode* DerivationTree::getRoot()
{
	return root;
}


//----------------------------Node Object Methods-----------------------------

DerivationTreeNode::DerivationTreeNode()
{
	this->type = UNDEFINED_NODE;
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

DerivationTreeNode::DerivationTreeNode(std::string intype, Vector3 inpos, Vector3 inextents, Quaternion inorient, std::vector<DerivationTreeNode>* inchildren, DerivationTreeNode* inparent)
{
	type = intype;
	position = Vector3(inpos);
	extents = Vector3(inextents);
	orientation = Quaternion(inorient);
	if (inchildren != NULL)
	{
		children = std::vector<DerivationTreeNode>(inchildren->begin(), inchildren->end());	
	}
	parent = inparent;
}

DerivationTreeNode::~DerivationTreeNode()
{
	for (std::vector<DerivationTreeNode>::iterator i = children.begin(); i != children.end(); i++)
	{
		//do nothing?
	}

	//do nothing since there were no new's used in the creation of this?
}

void DerivationTreeNode::scaleNode(Vector3 factor)
{
	//a new temp node that will become the child
	DerivationTreeNode temp = DerivationTreeNode(this);
	//copy the children to the children node
	temp.children = std::vector<DerivationTreeNode>(this->children);
	//and then remove them from this node
	this->children.clear();
	
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

		if (axis == 'x') //This is just temp to shut up an annoying error. It can be removed later
		{
			
		}

		//derive the children split nodes from the parent

		this->children.push_back(temp);
	}
}

void DerivationTreeNode::moveNode(Vector3 pos)
{
	//a new temp node that will become the child
	DerivationTreeNode temp = DerivationTreeNode(this);
	//copy the children to the children node
	temp.children = std::vector<DerivationTreeNode>(this->children);
	//and then remove them from this node
	this->children.clear();
	
	temp.position += pos;

	this->children.push_back(temp);
}

void DerivationTreeNode::rotateNode(Quaternion rot)
{
	DerivationTreeNode temp = DerivationTreeNode(this);
	temp.children = std::vector<DerivationTreeNode>(this->children);
	this->children.clear();
	
	temp.orientation = temp.orientation * rot; //FIXME: Is this right to apply the rotation?

	this->children.push_back(temp);
}

void DerivationTreeNode::addPrimitive(std::string intype, Vector3 pos, Vector3 ext, Quaternion orient)
{
	DerivationTreeNode temp;
	temp.type = intype;
	temp.position = pos;
	temp. extents = ext;
	temp.orientation = orient;

	this->children.push_back(temp);
}

void DerivationTreeNode::removeNode()
{
	DerivationTreeNode temp = DerivationTreeNode(this);

	temp.type = UNDEFINED_NODE;

	this->children.push_back(temp);
}

std::string DerivationTreeNode::displayNode(int n)
{
	std::string ret = "";
	
	//output appropriate amount of whitespace first
	for (int i = 0; i < n; i++)
	{
		ret += "\t";
	}
	
	//output this node's contents
	ret = ret + "Type: " + type +
		+ " Position: "
		+ Utility::numToString(position.x) + " "
		+ Utility::numToString(position.y) + " "
		+ Utility::numToString(position.z) + " "
		+ " Extents: "
		+ Utility::numToString(extents.x) + " "
		+ Utility::numToString(extents.y) + " "
		+ Utility::numToString(extents.z) + " ";

	ret += "\n";

	for (std::vector<DerivationTreeNode>::iterator i = children.begin(); i != children.end(); i++)
	{
		ret += i->displayNode(n + 1);
	}

	return ret;
}
