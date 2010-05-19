/*

 */

#include "GrammarRule.h"

GrammarRule::GrammarRule()
{
	lhs = "";
	rhs.clear();
}

GrammarRule::~GrammarRule()
{
	
}

//----------------------Symbol Methods-------------------------

Symbol::Symbol()
{
	name = "";
	factor = 0.0;
	num = intype = 0;
	axis = '\0';
	pos = ext = Ogre::Vector3();
	rot = orient = Ogre::Quaternion();
}

Symbol::Symbol(std::string inname)
{
	name = inname;
	factor = 0.0;
	num = intype = 0;
	axis = '\0';
	pos = ext = Ogre::Vector3();
	rot = orient = Ogre::Quaternion();
}

Symbol::~Symbol()
{

}
