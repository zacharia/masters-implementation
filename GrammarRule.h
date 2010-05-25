/*
  Name: GrammarRule.h
  
  Description:
  <description here>

  */

#ifndef _GRAMMARRULE_H
#define _GRAMMARRULE_H

//standard library inclusions
#include <vector>
#include <string>
#include <OGRE/Ogre.h>

//project inclusions
#include "Utility.h"

//these are used to define what string is associated with each symbol type.
#define RECTANGLE_NODE "rectangle"
#define CYLINDER_NODE "cylinder"
#define SPHERE_NODE "sphere"

#define SCALE_NODE "scale"
#define SPLIT_NODE "split"
#define MOVE_NODE "move"
#define ROTATE_NODE "rotate"
#define ADD_PRIMITIVE "add"
#define REMOVE_NODE "remove"

class Symbol
{
public:
	std::string name;
	Ogre::Vector3 factor;
	int num;
	char axis;
	Ogre::Vector3 pos;
	Ogre::Quaternion rot;
	std::string intype;
	Ogre::Vector3 ext;
	Ogre::Quaternion orient;

public:
	Symbol();

	~Symbol();

	Symbol(std::string inname);

	std::string toString();

	//bool operator==(Symbol& comp) const;
};

class GrammarRule
{
public:
	Symbol lhs;
	std::vector<Symbol> rhs;

	std::string name;

	double probability;

public:
	GrammarRule();

	~GrammarRule();

	std::string toString();

	//bool operator==(GrammarRule& comp); //const;
};

#endif
