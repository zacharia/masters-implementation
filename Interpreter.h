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

using namespace std;

class Interpreter
{
	//private stuff

	VoxelGrid* vg;

	vector<GrammarRule> rules;

public:
	Interpreter();

	~Interpreter();

	void setVoxelGrid(VoxelGrid* in);

	void interpretFileLsystem(string filename);

	void interpretFile(string filename);
};

#endif
