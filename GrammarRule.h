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
// HERE

class Symbol
{
public:
	std::string name;
	double factor;
	int num;
	char axis;
	Ogre::Vector3 pos;
	Ogre::Quaternion rot;
	int intype;
	Ogre::Vector3 ext;
	Ogre::Quaternion orient;

public:
	Symbol();

	~Symbol();

	Symbol(std::string inname);
};

class GrammarRule
{
public:
	std::string lhs;
	std::vector<std::string> rhs;

	std::string name;

public:
	GrammarRule();

	~GrammarRule();
};

#endif
