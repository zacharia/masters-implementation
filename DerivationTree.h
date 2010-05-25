// ---------------------------------------------------------------------------
// DerivationTree.H
// <very brief class description>
//
// Author: Zacharia Crumley
// ---------------------------------------------------------------------------
#ifndef _DERIVATIONTREE_H
#define _DERIVATIONTREE_H

#include <vector>

#include "OgreDisplay.h"
#include "GrammarRule.h"

#define UNDEFINED -1
#define EMPTY 0
#define RECTANGLE 2
#define CYLINDER 3
#define SPHERE 4

class DerivationTreeNode
{
	//the location of this node's shape (the center), relative to the parent shape
	Vector3 position;
	//the extents of this shape's scope (i.e. it's dimensions)
	Vector3 extents;
	//the orientation of the shape
	Quaternion orientation;
	
	//what type of shape is at this node (see the #defines above)
	int type;

	//a pointer to the parent of this node
	DerivationTreeNode* parent;

	//the children of this node
	std::vector<DerivationTreeNode> children;
	
public:
	DerivationTreeNode();

	DerivationTreeNode(DerivationTreeNode* in, bool copyChildren = false);

	DerivationTreeNode(std::string intype,
			   Vector3 inpos = Vector3::ZERO,
			   Vector3 inextents = Vector3::UNIT_SCALE,
			   Quaternion inorient = Quaternion::IDENTITY,
			   std::vector<DerivationTreeNode>* inchildren = NULL,
			   DerivationTreeNode* inparent = NULL);

	~DerivationTreeNode();

	void scaleNode(Vector3 factor);

	void splitNode(int num, char axis);

	void moveNode(Vector3 pos);

	void rotateNode(Quaternion rot);

	void addPrimitive(int intype, Vector3 pos, Vector3 ext, Quaternion orient);

	void removeNode();

	std::string displayNode(int n = 0);
};

class DerivationTree
{
	DerivationTreeNode* root;
	
public:
	DerivationTree();

	~DerivationTree();

	void initialize(DerivationTreeNode r);

	std::string displayTree();
};

#endif
