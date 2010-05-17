// ---------------------------------------------------------------------------
// DerivationTree.H
// <very brief class description>
//
// Author: Zacharia Crumley
// ---------------------------------------------------------------------------
#ifndef _DERIVATIONTREE_H
#define _DERIVATIONTREE_H

#include <vector>

class DerivationTreeNode
{
public:
	DerivationTreeNode();

	~DerivationTreeNode();
};

class DerivationTree
{
	DerivationTreeNode* root;
	
public:
	DerivationTree();

	~DerivationTree();
};

#endif
