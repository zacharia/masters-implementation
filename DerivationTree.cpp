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

void DerivationTree::createPrimitives(OgreDisplay* in)
{
	root->createPrimitives(in);
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
	this->extents = Vector3(in->extents);
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

DerivationTreeNode* DerivationTreeNode::scaleNode(Vector3 factor)
{
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

	//return the newly created child node in case there are further things to do in this rule
	return &*(--(this->children.end()));
}

//NOTE: char is either 'x', 'y' or 'z' and indicates along which local axis to split the node
//    not everything will necessarily obey the axis thing, e.g. cylinders can only split along their x or z axes
DerivationTreeNode* DerivationTreeNode::splitNode(int num, char axis)
{
	DerivationTreeNode temp;
	
	for (int i = 0; i < num; i++)
	{
		temp = DerivationTreeNode(this);

		if (axis == 'x')
		{
			temp.extents.x = this->extents.x / num;
			temp.position.x = (this->position.x - this->extents.x) + temp.extents.x
				+ (i * temp.extents.x * 2.0);
		}
		else if (axis == 'y')
		{
			
		}
		else if (axis == 'z')
		{
			
		}

		//derive the children split nodes from the parent

		this->children.push_back(temp);
	}
	this->active = false;

	return this;
}

DerivationTreeNode* DerivationTreeNode::moveNode(Vector3 pos)
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

	return &*(--(this->children.end()));
}

DerivationTreeNode* DerivationTreeNode::rotateNode(Quaternion rot)
{
	DerivationTreeNode temp = DerivationTreeNode(this);
	temp.children = std::vector<DerivationTreeNode>(this->children.begin(), this->children.end());
	this->children.clear();
	
	temp.orientation = temp.orientation * rot; //FIXME: Is this right to apply the rotation?

	this->children.push_back(temp);

	this->active = false;

	return &*(--(this->children.end()));
}

DerivationTreeNode* DerivationTreeNode::addPrimitive(std::string intype, Vector3 pos, Vector3 ext, Quaternion orient)
{
	DerivationTreeNode temp;
	temp.type = intype;
	temp.position = pos;
	temp. extents = ext;
	temp.orientation = orient;

	this->children.push_back(temp);

	return this;
}

DerivationTreeNode* DerivationTreeNode::removeNode()
{
	//I commented these out since I think they're pretty much redundant with the active flag added.
	// DerivationTreeNode temp = DerivationTreeNode(this);
	// temp.type = UNDEFINED_NODE;
	// this->children.push_back(temp);

	this->active = false;

        return this; //should this be the parent rather?
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
	//if we're not given a tree to look at, return NULL
	if (target == NULL)
	{
		return NULL;
	}
	//if the node we're given matches, and is active, return it
	if ((target->type == search) && (target->isActive()))
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

//returns the address of the newly derived node
DerivationTreeNode* DerivationTreeNode::applySymbol(Symbol* in)
{
	if (in->name == SCALE_NODE)
	{
		return this->scaleNode(in->factor);
	}
	else if (in->name == SPLIT_NODE)
	{
		return this->splitNode(in->num, in->axis);
	}
	else if (in->name == MOVE_NODE)
	{
		return this->moveNode(in->pos);
	}
	else if (in->name == ROTATE_NODE)
	{
		return this->rotateNode(in->rot);
	}
	else if (in->name == ADD_PRIMITIVE)
	{
		return this->addPrimitive(in->intype, in->pos, in->ext, in->orient);
	}
	else if ((in->name == RECTANGLE_NODE) || (in->name == CYLINDER_NODE) || (in->name == SPHERE_NODE))
	{
		return this->addPrimitive(in->name, in->pos, in->ext, in->orient);
	}
	else if (in->name == REMOVE_NODE)
	{
		return this->removeNode();
	}
	else
	{
		return NULL;
	}
}

void DerivationTreeNode::createPrimitives(OgreDisplay* in)
{
	//if it's an active node
	if (this->isActive())
	{
		//make the primitive of it
		if (this->type == RECTANGLE_NODE)
		{
			in->addCube(this->position, this->extents, this->orientation);
			//std::cout << "adding rectangle" << "\n"; //TEMP 
		}
		else if (this->type == CYLINDER_NODE)
		{
			in->addCylinder(this->position, this->extents, this->orientation);
			//std::cout << "adding cylinder" << "\n"; //TEMP 
		}
		else if (this->type == SPHERE_NODE)
		{
			
		}
	}

	//recurse on it's children.
	for (std::vector<DerivationTreeNode>::iterator i = this->children.begin(); i != this->children.end(); i++)
	{
		i->createPrimitives(in);
	}
}
