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

//argument variables
string infile = "";

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
	}

	//voxel grid init

	vg = new VoxelGrid(100, 100, 100);
	
	vg->initializeVoxelGrid(0x00);

	vg->makeRectangle(Ogre::Vector3(10,10,10), Ogre::Vector3(5, 5, 5), true);

	//interpreter initialization

	interpret = new Interpreter();
	interpret->setVoxelGrid(vg);

	//interpret the input file if we've been given one
	if (infile != "")
	{
		interpret->interpretFile(infile);
	}
	 
	//ogre init

	display = new OgreDisplay();
	display->initialize();
	display->createVoxelMesh();

	for (int i = 0; i < vg->getXSize(); i++)
	{
		for (int j = 0; j < vg->getYSize(); j++)
		{
			for (int k = 0; k < vg->getZSize(); k++)
			{
				if (vg->getValue(i,j,k) == 0x01)
				{
					//display->addVoxel(Ogre::Vector3(i,j,k));
					//display->addVoxelStatic(Ogre::Vector3(i,j,k));
					display->addVoxelBillboard(Ogre::Vector3(i,j,k));
				}
			}
		}
	}
	//display->addVoxelStatic(Ogre::Vector3(0,0,0));
	//display->buildStaticGeometry();
	//display->addVoxelBillboard(Ogre::Vector3(0,0,0));

	display->addCube(Vector3(0,0,0));
	display->addCube(Vector3(0,-10,0), Vector3(3,1,1), Quaternion(Radian(2),Vector3(0,1,0)) );
	display->addCylinder(Vector3(14,-5,0), Vector3(3,2,1), Quaternion(Radian(2),Vector3(0,1,2)));

	//OIS init

	input = InputManager::getSingletonPtr();
	input->initialise(display->getRenderWindow());
	
	while (run)
	{
		tick();
		//draw frame
	}

	if (vg != NULL)
	{
		delete vg;
	}
	if (input != NULL)
	{
		input->removeAllListeners();
		delete input;
	}
	if (display != NULL)
	{
		delete display;
	}
	if (interpret != NULL)
	{
		delete interpret;
	}

	cout << "\nEND OF PROGRAM\n";
	return 0;
}
