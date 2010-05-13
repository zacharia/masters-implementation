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

using namespace std;

struct Rule
{
	string lhs;
	vector<string> rhs;

	Rule()
	{
		lhs = "";
		rhs.clear();
	}
};

class Interpreter
{
	//private stuff

	VoxelGrid* vg;

	vector<Rule> rules;

public:
	Interpreter();

	~Interpreter();

	void setVoxelGrid(VoxelGrid* in);

	void interpretFileLsystem(string filename);

	void interpretFile(string filename);
};

#endif
