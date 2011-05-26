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
#include <OGRE/Ogre.h>

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

	static std::string ogreVector3ToString(Ogre::Vector3 in);

	static std::string ogreMatrix3ToString(Ogre::Matrix3 in, bool on_one_line = false);
};

#endif
