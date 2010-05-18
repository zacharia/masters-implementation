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

