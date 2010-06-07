/*

 */

#include "GrammarRule.h"

GrammarRule::GrammarRule()
{
	lhs = Symbol();
	rhs.clear();
	name = "";
	totalProbability = 0.0;
}

GrammarRule::~GrammarRule()
{
	
}

//NOTE: This method has not been properly tested. It may not work.
std::string GrammarRule::toString()
{
	std::string ret = "";

	ret = ret + lhs.toString() + "[" + Utility::numToString(this->totalProbability) + "]";
	
	for (std::vector<Expansion>::iterator j = rhs.begin(); j != rhs.end(); j++)
	{
		ret = ret + " -> ";
		
		for (std::vector<Symbol>::iterator i = j->expansion.begin(); i != j->expansion.end(); i++)
		{
			ret = ret + i->toString() + " ";
		}

		ret = ret + ": " + Utility::numToString(j->probability);

		ret = ret + "\n";
	}
	ret = ret + ";\n\n";
	
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
	factor = Ogre::Vector3();
	num = 0;
	intype = "";
	axis = '\0';
	pos = ext = Ogre::Vector3();
	rot = orient = Ogre::Quaternion();
}

Symbol::Symbol(std::string inname)
{
	name = inname;
	factor = Ogre::Vector3();
	num = 0;
	intype = "";
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
	ret = ret + this->name;

	ret = ret + "(";

	if (this->name == SCALE_NODE)
	{
		ret = ret + Utility::numToString(factor.x) + ", " + Utility::numToString(factor.y) + ", " + Utility::numToString(factor.z);
	}
	if (this->name == SPLIT_NODE)
	{
		ret = ret + "not_implemented_yet";
	}
	if (this->name == MOVE_NODE)
	{
		ret = ret + Utility::numToString(pos.x) + ", " + Utility::numToString(pos.y) + ", " + Utility::numToString(pos.z);
	}
	if (this->name == ROTATE_NODE)
	{
		ret = ret + Utility::numToString(rot.w) + ", " + Utility::numToString(rot.x) + ", " + Utility::numToString(rot.y) + ", " + Utility::numToString(rot.z);
	}
	if (this->name == ADD_PRIMITIVE)
	{
		ret = ret + intype + ",  " +
			Utility::numToString(pos.x) + ", " + Utility::numToString(pos.y) + ", " + Utility::numToString(pos.z) + ",  " +
			Utility::numToString(factor.x) + ", " + Utility::numToString(factor.y) + ", " + Utility::numToString(factor.z) + ",  " +
			Utility::numToString(rot.w) + ", " + Utility::numToString(rot.x) + ", " + Utility::numToString(rot.y) + ", " + Utility::numToString(rot.z);;
	}

	ret = ret + ")";
	
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
