#include "Interpreter.h"

#include "lex.yy.c"

Interpreter::Interpreter()
{
	vg = NULL;
	rules = vector<GrammarRule>();
	maxIterations = -1;
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

	GrammarRule currRule = GrammarRule();
	//this stores whether we still need to assign the LHS of the current rule (if it's false, symbols should be added to the RHS of the current rule)
	bool lhs = true;
	bool startSymbolNext = false;
		
	FILE* input = fopen(filename.c_str(), "r");
	yyrestart(input);

	int current = yylex();
	while (current != -1)
	{
		//cout << current << " |" << yytext << "|\n";
		if (current == SYMBOL)
		{
			//cout << "symbol.\n";
			
			if (// (currRule.lhs.name == "") &&
			    (lhs))
			{
				currRule.lhs = Symbol(yytext);
				lhs = false;
			}
			else
			{
				currRule.rhs.push_back(Symbol(yytext));
			}
		}
		else if (current == OPEN_BRACKET)
		{
			//cout << "open bracket.\n";
			string lastSymbolType;
			Symbol* lastSymbol = NULL;

			if (startSymbolNext)
			{
				lastSymbolType = startSymbol.name;
				lastSymbol = &startSymbol;
				startSymbolNext = false;
			}
			else if (lhs)
			{
				lastSymbolType = currRule.lhs.name;
				lastSymbol = &currRule.lhs;
			}
			else
			{
				lastSymbolType = currRule.rhs.back().name;
				lastSymbol = &currRule.rhs.back();
				cout << lastSymbolType << "\n";
			}

			if (lastSymbolType == SCALE_NODE)
			{
				yylex();
				lastSymbol->factor.x = atof(yytext);
				yylex(); //read the ,
				yylex();
				lastSymbol->factor.y = atof(yytext);
				yylex(); //read the ,
				yylex();
				lastSymbol->factor.z = atof(yytext);
				yylex(); //read the )
			}
			else if (lastSymbolType == SPLIT_NODE)
			{
				yylex();
				lastSymbol->num = atoi(yytext);
				yylex(); //read the ,
				yylex();
				lastSymbol->axis = yytext[0];
				yylex(); //read the )
			}
			else if (lastSymbolType == MOVE_NODE)
			{
				yylex();
				lastSymbol->pos.x = atof(yytext);
				yylex(); //read the ,
				yylex();
				lastSymbol->pos.y = atof(yytext);
				yylex(); //read the ,
				yylex();
				lastSymbol->pos.z = atof(yytext);
				yylex(); //read the )
			}
			else if (lastSymbolType == ROTATE_NODE)
			{
				double tempAngle;
				yylex();
				tempAngle = atof(yytext);
				yylex(); //read the ,
				Vector3 temp;
				yylex();
				temp.x = atof(yytext);
				yylex(); //read the ,
				yylex();
				temp.y = atof(yytext);
				yylex(); //read the ,
				yylex();
				temp.z = atof(yytext);
				yylex(); //read the )
				lastSymbol->rot = Ogre::Quaternion(Ogre::Radian(tempAngle), Ogre::Vector3(temp));
			}
			else if (lastSymbolType == ADD_PRIMITIVE)
			{
				//read in what type of shape to add
				yylex();
				lastSymbol->intype = yytext;
				yylex(); //read the ,

				//read in it's position relative to the parent
				Vector3 temp;
				yylex();
				temp.x = atof(yytext);
				yylex(); //read the ,
				yylex();
				temp.y = atof(yytext);
				yylex(); //read the ,
				yylex();
				temp.z = atof(yytext);
				yylex(); //read the )
				lastSymbol->pos = Vector3(temp);

				//read in it's extents
				yylex();
				temp.x = atof(yytext);
				yylex(); //read the ,
				yylex();
				temp.y = atof(yytext);
				yylex(); //read the ,
				yylex();
				temp.z = atof(yytext);
				yylex(); //read the )
				lastSymbol->ext = Vector3(temp);

				//read in it's orientation as a Quaternion
				yylex();
				double tempType = atof(yytext);
				yylex(); //read the ,
				yylex();
				temp.x = atof(yytext);
				yylex(); //read the ,
				yylex();
				temp.y = atof(yytext);
				yylex(); //read the ,
				yylex();
				temp.z = atof(yytext);
				yylex(); //read the )
				lastSymbol->orient = Ogre::Quaternion(Ogre::Radian(tempType), Ogre::Vector3(temp));
			}
			else if ((lastSymbolType == RECTANGLE_NODE) || (lastSymbolType == CYLINDER_NODE) || (lastSymbolType == SPHERE_NODE))
			{
				lastSymbol->intype = lastSymbolType;
				
				//read in it's position relative to the parent
				Vector3 temp;
				yylex();
				temp.x = atof(yytext);
				yylex(); //read the ,
				yylex();
				temp.y = atof(yytext);
				yylex(); //read the ,
				yylex();
				temp.z = atof(yytext);
				yylex(); //read the )
				lastSymbol->pos = Vector3(temp);

				//read in it's extents
				yylex();
				temp.x = atof(yytext);
				yylex(); //read the ,
				yylex();
				temp.y = atof(yytext);
				yylex(); //read the ,
				yylex();
				temp.z = atof(yytext);
				yylex(); //read the )
				lastSymbol->ext = Vector3(temp);

				//read in it's orientation as a Quaternion
				yylex();
				double tempType = atof(yytext);
				yylex(); //read the ,
				yylex();
				temp.x = atof(yytext);
				yylex(); //read the ,
				yylex();
				temp.y = atof(yytext);
				yylex(); //read the ,
				yylex();
				temp.z = atof(yytext);
				yylex(); //read the )
				lastSymbol->orient = Ogre::Quaternion(Ogre::Radian(tempType), Ogre::Vector3(temp));
			}
			else if (lastSymbolType == REMOVE_NODE)
			{
				//should I add a parameter for whether to delete the children?
			}
		}
		else if (current == ASSIGN)
		{
			//cout << "assign.\n";
			lhs = false;
		}
		else if (current == RULE_SEPARATOR)
		{
			//cout << "rule separator.\n";
			
			//add the last rule to the rule set
			rules.push_back(currRule);
			//reinitialize the currRule var to take the next rule
			currRule = GrammarRule();
			lhs = true;
		}
		else if (current == START_SYMBOL)
		{
			//cout << "start symbol.\n";
			
			yylex();
			startSymbol.name = yytext;
			startSymbolNext = true;
		}
		else if (current == PROBABILITY)
		{
			//cout << "probability symbol.\n";

			yylex();
			currRule.probability = atof(yytext);
		}
		current = yylex();
	}

	cout << "\n\n===RULES===\n";
	cout << "START SYMBOL: " << startSymbol.toString() << "\n";
	cout << "NORMAL SYMBOLS: \n";
	for (vector<GrammarRule>::iterator i = rules.begin(); i != rules.end(); i++)
	{
		cout << i->toString() << "\n";
	}

	DerivationTreeNode root = DerivationTreeNode(startSymbol.name, startSymbol.pos, startSymbol.ext);
	derTree = DerivationTree();
	derTree.initialize(root);

	this->deriveTree();

	cout << "\nLE TREE:\n\n" << derTree.displayTree();
	
	delete input;
}

/*
  Do the derivation of the tree.
  Assumes that everything has been set up through all the other methods in this class.
 */
void Interpreter::deriveTree()
{
	int iterations = 0;
	DerivationTreeNode* target = NULL;
		
	while (treeHasNonTerminals(derTree.getRoot()) &&
	       underMaxIterations(iterations))
	{
		for (vector<GrammarRule>::iterator i = rules.begin(); i != rules.end(); i++)
		{
			target = derTree.findNode(i->lhs.name);
			if (target != NULL)
			{
				for (vector<Symbol>::iterator j = i->rhs.begin(); j != i->rhs.end(); j++)
				{
					//update target to point at the node it should modify for the symbol in this rule					
					target = target->applySymbol(&*j);
				}
				break; //this prevents more than one derivation happening per iteration. It breaks out of the for loop and back into the while loop
			}
		}
		iterations++;
	}
}

void Interpreter::createPrimitives()
{
	derTree.createPrimitives(display);
}

void Interpreter::setDisplay(OgreDisplay* in)
{
	display = in;
}

bool Interpreter::treeHasNonTerminals(DerivationTreeNode in)
{
	vector<std::string> nonTerminals;
	for (std::vector<GrammarRule>::iterator i = rules.begin(); i != rules.end(); i++)
	{
		nonTerminals.push_back((i->lhs).name);
	}
	
	if (in.children.empty())
	{
		for (vector<std::string>::iterator i = nonTerminals.begin(); i != nonTerminals.end(); i++)
		{
			if (*i == in.type)
			{
				return true;
			}
		}
		return false;
	}
	else
	{
		for (vector<DerivationTreeNode>::iterator i = in.children.begin(); i != in.children.end(); i++)
		{
			return treeHasNonTerminals(*i);
		}
		return false;
	}
}

bool Interpreter::underMaxIterations(int iterations)
{
	if (maxIterations > 0)
	{
		if (iterations < maxIterations)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
	
	// if ((iterations < maxIterations) && (maxIterations > 0))
	// {
	// 	return true;
	// }
	// else
	// {
	// 	return false;
	// }	
}

void Interpreter::setMaxIterations(int in)
{
	maxIterations = in;
}

int Interpreter::getMaxIterations()
{
	return maxIterations;
}
