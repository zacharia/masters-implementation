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

DerivationTreeNode* DerivationTree::findNode(std::string search)
{
	if (root == NULL)
	{
		return NULL;
	}
	else
	{
		return root->findNode(search, root);
	}
}

bool DerivationTree::isRootNull()
{
	if (root == NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}


//----------------------------Node Object Methods-----------------------------

DerivationTreeNode::DerivationTreeNode()
{
	this->type = UNDEFINED_NODE;
	this->position = Vector3(0,0,0);
	this->extents = Vector3(0,0,0);
	this->orientation = Quaternion::IDENTITY;

	this->parent = NULL;

	this->active = true;

	children = std::vector<DerivationTreeNode>();
}

DerivationTreeNode::DerivationTreeNode(DerivationTreeNode* in, bool copyChildren)
{
	this->position = in->position;
	this->extents = in->extents;
	this->orientation = in->orientation;
	this->type = in->type;
	this->parent = NULL;
	this->active = in->active;
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
	this->active = true;
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
	std::cout << "active: " << this->active << "\n"; //TEMP
	
	//a new temp node that will become the child
	DerivationTreeNode temp = DerivationTreeNode(this);
	//copy the children to the children node
	temp.children = std::vector<DerivationTreeNode>(this->children.begin(), this->children.end());
	//and then remove them from this node
	//this->children.clear();
	this->children = std::vector<DerivationTreeNode>();
	
	temp.extents.x *= factor.x;
	temp.extents.y *= factor.y;
	temp.extents.z *= factor.z;

	this->children.push_back(temp);
	this->active = false;
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
	this->active = false;
}

void DerivationTreeNode::moveNode(Vector3 pos)
{
	//a new temp node that will become the child
	DerivationTreeNode temp = DerivationTreeNode(this);
	//copy the children to the children node
	temp.children = std::vector<DerivationTreeNode>(this->children.begin(), this->children.end());
	//and then remove them from this node
	this->children.clear();
	
	temp.position += pos;

	this->children.push_back(temp);

	this->active = false;
}

void DerivationTreeNode::rotateNode(Quaternion rot)
{
	DerivationTreeNode temp = DerivationTreeNode(this);
	temp.children = std::vector<DerivationTreeNode>(this->children.begin(), this->children.end());
	this->children.clear();
	
	temp.orientation = temp.orientation * rot; //FIXME: Is this right to apply the rotation?

	this->children.push_back(temp);

	this->active = false;
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

	this->active = false;
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
		+ Utility::numToString(extents.z) + " "
		+ " Active: " + Utility::numToString(active);

	ret += "\n";

	for (std::vector<DerivationTreeNode>::iterator i = children.begin(); i != children.end(); i++)
	{
		ret += i->displayNode(n + 1);
	}

	return ret;
}

DerivationTreeNode* DerivationTreeNode::findNode(std::string search, DerivationTreeNode* target)
{
	if (target == NULL)
	{
		return NULL;
	}
	if (target->type == search)
	{
		return target;
	}
	else
	{
		if (target->children.empty())
		{
			return NULL;
		}
		else
		{
			DerivationTreeNode* ret = NULL;
			for (std::vector<DerivationTreeNode>::iterator i = target->children.begin(); i != target->children.end(); i++)
			{
				ret = findNode(search, &(*i));
				if ((ret != NULL) && (ret->isActive()))
				{
					return ret;
				}				
			}
			return ret;
		}
	}
}

bool DerivationTreeNode::isActive()
{
	return this->active;
}

void DerivationTreeNode::applySymbol(Symbol* in)
{
	if (in->name == SCALE_NODE)
	{
		this->scaleNode(in->factor);
	}
	else if (in->name == SPLIT_NODE)
	{
		this->splitNode(in->num, in->axis);
	}
	else if (in->name == MOVE_NODE)
	{
		this->moveNode(in->pos);
	}
	else if (in->name == ROTATE_NODE)
	{
		this->rotateNode(in->rot);
	}
	else if (in->name == ADD_PRIMITIVE)
	{
		this->addPrimitive(in->intype, in->pos, in->ext, in->orient);
	}
	else if ((in->name == RECTANGLE_NODE) || (in->name == CYLINDER_NODE) || (in->name == SPHERE_NODE))
	{
		this->addPrimitive(in->name, in->pos, in->ext, in->orient);
	}
	else if (in->name == REMOVE_NODE)
	{
		this->removeNode();
	}
	else
	{
		
	}
}
