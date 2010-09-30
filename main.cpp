/*

A comment

*/

//standard library inclusions
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

//project inclusions
#include "VoxelGrid.h"
#include "InputManager.h"
#include "OgreDisplay.h"
#include "Interpreter.h"

using namespace std;

VoxelGrid* vg = NULL;
OgreDisplay* display = NULL;
InputManager* input = NULL;
//used to tell when to stop running the game loop
bool run = true;
//store mouse positions
int mouseX=100, mouseY=100;
//store the change in mouse positions each tick
int mouseDeltaX = 0, mouseDeltaY = 0;
Interpreter* interpret = NULL;
int maxIterations = -1;

//argument variables
string infile = "";
unsigned int voxel_grid_size = 1024;
bool display_axes = false;

void tick()
{
	//keyboard input handling
	
	input->capture();
	if (input->getKeyboard()->isKeyDown(OIS::KC_ESCAPE))
	{		
		run = false;
	}
	if (input->getKeyboard()->isKeyDown(OIS::KC_W))
	{		
		display->getCamera()->moveRelative(Vector3(0.0,0.0,-1.0));
	}
	if (input->getKeyboard()->isKeyDown(OIS::KC_S))
	{		
		display->getCamera()->moveRelative(Vector3(0.0,0.0,1.0));
	}
	if (input->getKeyboard()->isKeyDown(OIS::KC_A))
	{		
		display->getCamera()->moveRelative(Vector3(-1.0,0.0,0.0));
	}
	if (input->getKeyboard()->isKeyDown(OIS::KC_D))
	{		
		display->getCamera()->moveRelative(Vector3(1.0,0.0,0.0));
	}
	if (input->getKeyboard()->isKeyDown(OIS::KC_SPACE))
	{		
		display->getCamera()->move(Vector3(0.0,1.0,0.0));
	}
	if (input->getKeyboard()->isKeyDown(OIS::KC_LCONTROL))
	{		
		display->getCamera()->move(Vector3(0.0,-1.0,0.0));
	}

	//cout << display->getCamera()->getPosition().x;

	//mouse handling

	//using rel seems to be the best way to handle it, abs causes problems around the edges.
	mouseDeltaX = input->getMouse()->getMouseState().X.rel;
	mouseDeltaY = input->getMouse()->getMouseState().Y.rel;
	if (mouseDeltaX != 0)
	{
		display->getCamera()->yaw(Ogre::Radian(mouseDeltaX * -0.01));
	}
	if (mouseDeltaY != 0)
	{
		display->getCamera()->pitch(Ogre::Radian(mouseDeltaY * -0.01));
	}
	
	//update world
	//EMPTY - to be filled later

	//draw new frame
	display->drawFrame();

	Ogre::WindowEventUtilities::messagePump();
}

int main(int argc, char** argv)
{
	cout << "Prototype voxel based thing. Cooler than ice.";

	// argument handling

	string curr;

	//go through the args and handle them
	for (int i = 0; i < argc; i++)
	{
		curr = argv[i];
		if (curr == "-i") //the input file to read
		{
			infile = argv[++i];
		}
		else if (curr == "-m") //maxIterations for derivation
		{
			maxIterations = atoi(argv[++i]);
		}
		else if (curr == "-s") //size of the octree for the voxel grid
		{
			voxel_grid_size = atoi(argv[++i]);
		}
		if (curr == "-f") //the already interpreted (by the python thing) file to use
		{
			infile = argv[++i];
		}
		if (curr == "-x") //display axes
		{
			display_axes = true;
		}
		if (curr == "-h") //display help
		{
			cout << "options:\n"
			     << "-i <file>\t the input file to interpret and use\n"
			     << "-m <num>\t the max number of iterations to do when interpreting\n"
			     << "-s <num>\t the size to make the voxel grid when doing voxel grid stuff\n"
			     << "-f <file>\t a file to use that was produced by the python interpreter\n"
			     << "-x\t\t display the x, y and z axes\n"
			     << "-h\t\t display this help information\n\n";
		}
	}
	
	//seed the random number generator
	srand(time(NULL));
		
	//ogre init
	display = new OgreDisplay();
	display->initialize();
	display->createVoxelMesh();

	if (display_axes)
	{
		display->addCube(Ogre::Vector3(0, 0, 0), Ogre::Vector3(0.25, 0.25, 0.25), Ogre::Quaternion::IDENTITY);
		display->addCube(Ogre::Vector3(0, 0, 0), Ogre::Vector3(0.1, 100, 0.1), Ogre::Quaternion(Ogre::Radian(M_PI), Ogre::Vector3(1,1,0)));
		display->addCube(Ogre::Vector3(0, 0, 0), Ogre::Vector3(0.1, 100, 0.1), Ogre::Quaternion(Ogre::Radian(M_PI), Ogre::Vector3(0,1,1)));
		display->addCube(Ogre::Vector3(0, 0, 0), Ogre::Vector3(0.1, 100, 0.1), Ogre::Quaternion::IDENTITY);
	}
	
	//interpret the input file if we've been given one
	if (infile != "")
	{
		//interpreter initialization
		interpret = new Interpreter();
		//interpret->setVoxelGrid(vg);
		interpret->setMaxIterations(maxIterations);
		interpret->interpretFile(infile);
		interpret->setDisplay(display);
		interpret->createPrimitives();
	}

	//voxel grid init
	vg = new VoxelGrid(voxel_grid_size);
	
	//OIS init
	input = InputManager::getSingletonPtr();
	input->initialise(display->getRenderWindow());

	//main game loop
	while (run)
	{
		tick();
		//draw frame
	}

	//memory clean up
	if (vg != NULL)
	{
		delete vg;
		vg = NULL;
	}
	if (input != NULL)
	{
		input->removeAllListeners();
		delete input;
		input = NULL;
	}
	if (display != NULL)
	{
		delete display;
		display = NULL;
	}
	if (interpret != NULL)
	{
		delete interpret;
		interpret = NULL;
	}

	cout << "\nEND OF PROGRAM\n";
	return 0;
}
