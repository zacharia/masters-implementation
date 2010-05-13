#include "Interpreter.h"

#include "lex.yy.c"

Interpreter::Interpreter()
{
	vg = NULL;
	rules = vector<Rule>();
}

Interpreter::~Interpreter()
{
	
}

//this method assigns the voxel grid we'll be working with
void Interpreter::setVoxelGrid(VoxelGrid* in)
{
	vg = in;
}

//this reads in a file, interprets it and does the creation stuff in the currently set voxel grid.
/*
  symbols:
  F - go forward by units indicated in arg
  x,y,z - do rotations about the associated axis (arguments in degrees)
  rectangle - make a rectangle at current location, x,y,z dimensions in args
  ellipsoid - make an ellipsoid at curr loc, dimensions in args
  cylinder - make a cylinder at curr loc, dim in args, 4th arg is axis
 */
void Interpreter::interpretFileLsystem(string filename)
{
	using namespace Ogre;

	//open up the file given in argument
	ifstream in(filename.c_str());
	if (in)
	{
		string currentSymbol = "";

		//these store the state, so that we can use [ and ] a la L-system turtle interpretation
		stack<Vector3> currentPosition;
		stack<Vector3> currentDirection;
		stack<bool> currentAddMode;

		//initial values to set the turtle to
		Vector3 currPos(vg->getXSize() / 2.0, vg->getYSize() / 2.0, vg->getZSize() / 2.0);
		Vector3 currDir(0.0, 1.0, 0.0);

		currentPosition.push(currPos);
		currentDirection.push(currDir);
		currentAddMode.push(true);

		//go through the file
		while (!in.eof())
		{
			//get the next symbol and depending on what it is, do something
			in >> currentSymbol >> ws;

			//moving the turtle forwards
			if (currentSymbol == "F")
			{
				double amount;
				string temp;
				in >> temp >> ws;
				amount = atof(temp.c_str());

				//update current position
				currentPosition.top() += currentDirection.top() * amount;
			}
			//doing rotations about the axes
			else if ((currentSymbol == "x") || (currentSymbol == "y") || (currentSymbol == "z"))
			{
				//read in the argument associated with the symbol
				double amount;
				string temp;
				in >> temp >> ws;
				amount = atof(temp.c_str());

				Quaternion rot;
				//get the correct Quaternion to use for the rotation
				if (currentSymbol == "x")
				{
					rot = Quaternion(Radian(Degree(amount)), Vector3::UNIT_X);
				}
				else if (currentSymbol == "y")
				{
					rot = Quaternion(Radian(Degree(amount)), Vector3::UNIT_Y);
				}
				else if (currentSymbol == "z")
				{
					rot = Quaternion(Radian(Degree(amount)), Vector3::UNIT_Z);
				}

				//update the current direction using the rotation
				currentDirection.top() = rot * currentDirection.top();
				currentDirection.top().normalise();
			}
			//handle pushing the state
			else if (currentSymbol == "[")
			{
				currentDirection.push(currentDirection.top());
				currentPosition.push(currentPosition.top());
				currentAddMode.push(currentAddMode.top());
			}
			//handle popping the state
			else if (currentSymbol == "]")
			{
				currentDirection.pop();
				currentPosition.pop();
				currentAddMode.pop();
			}
			else if (currentSymbol == "+")
			{
				currentAddMode.top() = true;
			}
			else if (currentSymbol == "-")
			{
				currentAddMode.top() = false;
			}
			else if ((currentSymbol == "rectangle") || (currentSymbol == "rectangle_subtract"))
			{
				//read in the arguments associated with the symbol
				double xSize, ySize, zSize;
				string temp;
				in >> temp >> ws;
				xSize = atof(temp.c_str());
				in >> temp >> ws;
				ySize = atof(temp.c_str());
				in >> temp >> ws;
				zSize = atof(temp.c_str());

				if (currentAddMode.top() == true)
				{
					vg->makeRectangle(currentPosition.top(), Vector3(xSize, ySize, zSize), true);       
				}
				else
				{
					vg->makeRectangle(currentPosition.top(), Vector3(xSize, ySize, zSize), false);
				}
			}
			else if ((currentSymbol == "ellipsoid") || (currentSymbol == "ellipsoid_subtract"))
			{
				//read in the arguments associated with the symbol
				double xSize, ySize, zSize;
				string temp;
				in >> temp >> ws;
				xSize = atof(temp.c_str());
				in >> temp >> ws;
				ySize = atof(temp.c_str());
				in >> temp >> ws;
				zSize = atof(temp.c_str());

				if (currentAddMode.top() == true)
				{
					vg->makeEllipsoid(currentPosition.top(), Vector3(xSize, ySize, zSize), true);	
				}
				else
				{
					vg->makeEllipsoid(currentPosition.top(), Vector3(xSize, ySize, zSize), false);
				}
			}
			else if ((currentSymbol == "cylinder") || (currentSymbol == "cylinder_subtract"))
			{
				//read in the arguments associated with the symbol
				double xSize, ySize, zSize;
				string temp, orientation;
				in >> temp >> ws;
				xSize = atof(temp.c_str());
				in >> temp >> ws;
				ySize = atof(temp.c_str());
				in >> temp >> ws;
				zSize = atof(temp.c_str());
				in >> orientation >> ws;

				if (currentAddMode.top() == true)
				{
					vg->makeCylinder(currentPosition.top(), Vector3(xSize, ySize, zSize), orientation[0], true);
				}
				else
				{
					vg->makeCylinder(currentPosition.top(), Vector3(xSize, ySize, zSize), orientation[0], false);
				}
			}
		}
	}
	else
	{
		cout << "Error occured opening file: " << filename << "\nExiting.\n";
	}
}

void Interpreter::interpretFile(string filename)
{
	using namespace Ogre;

	Rule temp = Rule();

	FILE* input = fopen(filename.c_str(), "r");
	yyrestart(input);

	int current = yylex();
	// cout << current << " " << yytext << "\n";
	// current = yylex();
	// cout << current << " " << yytext << "\n";
	//  current = yylex();
	// cout << current << " " << yytext << "\n";
	//  current = yylex();
	// cout << current << " " << yytext << "\n";
	// current = yylex();
	// cout << current << " " << yytext << "\n";
	// current = yylex();
	// cout << current << " " << yytext << "\n";
	// current = yylex();
	// cout << current << " " << yytext << "\n";
	// current = yylex();
	// cout << current << " " << yytext << "\n";
	while (current != -1)
	{
		cout << current << " |" << yytext << "|\n";
		if (current == SYMBOL)
		{
			
		}
		current == yylex();
		if (current == -1)
		{
			break;
		}
	}
}
