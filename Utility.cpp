#include "Utility.h"

std::string Utility::numToString(int in)
{
	std::stringstream convert;
	convert << in;
	return convert.str();
}

std::string Utility::numToString(double in)
{
	std::stringstream convert;
	convert << in;
	return convert.str();
}

std::string Utility::ogreVector3ToString(Ogre::Vector3 in)
{
	std::string ret = "";
	ret = numToString(in.x) + " " + numToString(in.y) + " " + numToString(in.z);
	return ret;
}

std::string Utility::ogreMatrix3ToString(Ogre::Matrix3 in, bool on_one_line)
{
	std::string ret = "";
	std::string line_end = "\n";
	if (on_one_line)
	{
		line_end = " ";
	}
	ret = numToString(in[0][0]) + " " + numToString(in[0][1]) + " " + numToString(in[0][2]) + line_end;
	ret = numToString(in[1][0]) + " " + numToString(in[1][1]) + " " + numToString(in[1][2]) + line_end;
	ret = numToString(in[2][0]) + " " + numToString(in[2][1]) + " " + numToString(in[2][2]);
	return ret;
}
