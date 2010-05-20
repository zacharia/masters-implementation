/*

 */

#include "GrammarRule.h"

GrammarRule::GrammarRule()
{
	lhs = Symbol();
	rhs.clear();
}

GrammarRule::~GrammarRule()
{
	
}

//NOTE: This method has not been properly tested. It may not work.
std::string GrammarRule::toString()
{
	std::string ret = "";

	ret = ret + lhs.toString() + " -> ";
	
	for (std::vector<Symbol>::iterator i = rhs.begin(); i != rhs.end(); i++)
	{
		ret = ret + i->toString() + " ";
	}

	ret = ret + ";\n";
	
	return ret;
}

//NOTE: This method has not been properly tested. It may not work.
//the const was commented out because it was causing a problematic compilation error
// bool GrammarRule::operator==(GrammarRule& comp) //const?
// {
// 	if ((this->lhs == comp.lhs) &&
// 	    (this->name == comp.name) &&
// 	    (this->rhs.size() == comp.rhs.size())
// 	    // (this->rhs == comp.rhs)
// 		)
// 	{
// 		std::vector<Symbol>::iterator i = comp.rhs.begin();
// 		for (std::vector<Symbol>::iterator j = rhs.begin(); j != rhs.end(); j++)
// 		{
// 			if (!(*i == *j))
// 			{
// 				return false;
// 			}
// 			i++;
// 		}
// 		return true;	
// 	}
// 	else
// 	{
// 		return false;
// 	}
// }

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

//NOTE: This method has not been properly tested. It may not work.
std::string Symbol::toString()
{
	std::string ret = "";
	ret = ret +
		this->name + " ";
		
	return ret;
}

//NOTE: This method has not been properly tested. It may not work.
// bool Symbol::operator==(Symbol& comp) const
// {
// 	if ((this->name == comp.name) &&
// 	    (this->factor == comp.factor) &&
// 	    (this->num == comp.num) &&
// 	    (this->axis == comp.axis) &&
// 	    (this->pos == comp.pos) &&
// 	    (this->rot == comp.rot) &&
// 	    (this->intype == comp.intype) &&
// 	    (this->ext == comp.ext) &&
// 	    (this->orient == comp.orient))
// 	{
// 		return true;
// 	}
// 	else
// 	{
// 		return false;
// 	}
// }
