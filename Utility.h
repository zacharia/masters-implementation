/*
  Name: Utility.h
  
  Description:
  A class of static utility methods and defines that don't fit into any other class well, but are needed by them.

  */

#ifndef _PROTOTYPEUTILITY_H
#define _PROTOTYPEUTILITY_H

//standard library inclusions
#include <sstream>
#include <string>

//project inclusions
// HERE

class Utility
{
private:


public:
	Utility();

	~Utility();

	static std::string numToString(int in);

	static std::string numToString(double in);
};

#endif
