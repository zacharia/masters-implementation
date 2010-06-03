// ---------------------------------------------------------------------------
//
// <filename>.H
// <very brief class description>
//
// Author: <original author>
// ---------------------------------------------------------------------------

#ifndef _INTERPRETER_H
#define _INTERPRETER_H

//standard library inclusions
#include <fstream>
#include <iostream>
#include <stack>
#include <vector>

//project inclusions
#include "VoxelGrid.h"
#include "DerivationTree.h"
#include "GrammarRule.h"
#include "OgreDisplay.h"

using namespace std;

class Interpreter
{
	//private stuff

	VoxelGrid* vg;

	vector<GrammarRule> rules;
	Symbol startSymbol;
	DerivationTree derTree;

	//stores the maximum number of grammar rules that can be executed
	int maxIterations;

	bool treeHasNonTerminals(DerivationTreeNode in);

	bool underMaxIterations(int iterations);

	OgreDisplay* display;

public:
	Interpreter();

	~Interpreter();

	void setVoxelGrid(VoxelGrid* in);

	void interpretFileLsystem(string filename);

	void interpretFile(string filename);

	void deriveTree();

	void createPrimitives();

	void setDisplay(OgreDisplay* in);

	void setMaxIterations(int in);

	int getMaxIterations();
};

#endif
