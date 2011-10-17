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
int maxIterations = -1;

//argument variables
string interpreted_file = "";
unsigned int voxel_grid_size = 1024;
bool display_axes = false;
double grid_granularity = 1.0;
unsigned int polygonize_chunk_size = 0;
bool point_rendering = false;

bool cameraLightKeyWasDown = false;
bool originLightKeyWasDown = false;

//this is used to indicate whether or not to scale the shapes into the octree's space.
bool scaleShapes = false;
bool drawBoundingBoxes = false;

bool verbose = false;
bool useMarchingCubes = true;

bool usePriorities = true;

string automata_rules_file = "";
string automata_rules_method = "";
int automata_num_iterations = -1;
int automata_neighbourhood_size = -1;

//empty string means don't actaully export
string exported_mesh_file_name = "";

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

	//screenshot key
	if (input->getKeyboard()->isKeyDown(OIS::KC_T))
	{		
		display->takeScreenshot();
	}

	//check for lighting control changes
	if (input->getKeyboard()->isKeyDown(OIS::KC_Q))
	{
		cameraLightKeyWasDown = true;
	}
	else
	{
		if (cameraLightKeyWasDown)
		{
			cameraLightKeyWasDown = false;
			display->toggleCameraLight();	
		}		
	}
	if (input->getKeyboard()->isKeyDown(OIS::KC_E))
	{
		originLightKeyWasDown = true;
	}
	else
	{
		if (originLightKeyWasDown)
		{
			originLightKeyWasDown = false;
			display->toggleOriginLight();	
		}		
	}
	//dimming ambient light
	if (input->getMouse()->getMouseState().buttonDown(OIS::MB_Left))
	{
		Ogre::ColourValue amb_light = display->getSceneManager()->getAmbientLight();
		amb_light.r -= 0.01;
		amb_light.g -= 0.01;
		amb_light.b -= 0.01;
		if (amb_light.r < 0)
			amb_light.r = 0;
		if (amb_light.g < 0)
			amb_light.g = 0;
		if (amb_light.b < 0)
			amb_light.b = 0;
		display->getSceneManager()->setAmbientLight(amb_light);
	}
	if (input->getMouse()->getMouseState().buttonDown(OIS::MB_Right))
	{
		Ogre::ColourValue amb_light = display->getSceneManager()->getAmbientLight();
		amb_light.r += 0.01;
		amb_light.g += 0.01;
		amb_light.b += 0.01;
		if (amb_light.r > 1.0)
			amb_light.r = 1;
		if (amb_light.g > 1.0)
			amb_light.g = 1;
		if (amb_light.b > 1.0)
			amb_light.b = 1;
		display->getSceneManager()->setAmbientLight(amb_light);
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
	// argument handling

	string curr;

	//go through the args and handle them
	for (int i = 0; i < argc; i++)
	{
		curr = argv[i];

		if ((curr == "--grid-size") || (curr == "-s")) //size of the octree for the voxel grid
		{
			voxel_grid_size = atoi(argv[++i]);
		}
		if ((curr == "--input-file") || (curr == "-f")) //the already interpreted (by the python thing) file to use
		{
			interpreted_file = argv[++i];
		}
		if ((curr == "--display-axes") || (curr == "-x")) //display axes
		{
			display_axes = !display_axes;
		}
		if ((curr == "--scale-shapes") || (curr == "-q")) //display axes
		{
			scaleShapes = true;
		}
		if ((curr == "--draw-bounding-boxes") || (curr == "-b"))
		{
			drawBoundingBoxes = true;
		}
		if ((curr == "--use-point-rendering") || (curr == "-p"))
		{
			point_rendering = true;
		}
		if ((curr == "--grid-granularity") || (curr == "-g")) //grid granularity
		{
			grid_granularity = atof(argv[++i]);
		}		
		if ((curr == "--verbose") || (curr == "-v"))
		{
			verbose = !verbose;
		}
		if ((curr == "--use-marching-tetrahedra") || (curr == "-t"))
		{
			useMarchingCubes = false;
		}
		if ((curr == "--ignore-priorities") || (curr == "-n"))
		{
			usePriorities = false;
		}
		//the python file that contains the cellular automata rules used to do the surface detailing
		if ((curr == "--automata-file") || (curr == "-r"))
		{
			automata_rules_file = argv[++i];
		}
		//the name of the method from the rules file to call on each voxel. Defaults to 'main' if this argument is not given
		if ((curr == "--automata-method") || (curr == "-m"))
		{
			automata_rules_method = argv[++i];
		}
		//the size of the neighbourhood to use when doing the detailing automata.
		if ((curr == "--automata-neighbourhood-size") || (curr == "-ans"))
		{
			automata_neighbourhood_size = atoi(argv[++i]); 
		}
		//the number of iterations of the cellular automata to do.
		if ((curr == "--automata-num-iterations") || (curr == "-ani"))
		{
			automata_num_iterations = atoi(argv[++i]); 
		}
		//the name of the file to export the the generated model.
		if ((curr == "--export-mesh") || (curr == "-o"))
		{
			exported_mesh_file_name = argv[++i];
		}
		if ((curr == "--help") || (curr == "-h")) //display help
		{
			cout << "options:\n"			     
			     << "-p\t\t use point rendering instead of mesh based.\n"
			     << "-s <num>\t the size to make the voxel grid when doing voxel grid stuff\n"
			     << "-f <file>\t a file to use that was produced by the python interpreter\n"
			     << "-x\t\t toggles displaying of the x, y and z axes\n"
			     << "-b\t\t display the bounding boxes of the octree grid and spacecraft.\n"
			     << "-g <num>\t the granularity to use when adding stuff to the voxel grid.\n"
			     << "-c <num>\t the size to make chunks when polygonizing the voxel grid.\n"
			     << "-q\t\t scale the shapes into the octree's coordinates.\n"
			     << "-v\t\t toggles enabling of verbose output.\n"
			     << "-t\t\t Use marching tetrahedra instead of the standard marching cubes.\n"
			     << "-n\t\t Ignore shape priorities when voxelizing them.\n"
			     << "-r <file>\t the python file containing the automata rule set to use when doing surface detail.\n"
			     << "-m <name>\t the name of the method to use from the automata rules file.\n"
			     << "-ans <num>\t the neighbourhood size to use when running the automata.\n"
			     << "-ani <num>\t the number of iterations of the automata rule set to do.\n"
			     << "-o <name>\t the name of the file to export the generated spacecraft model to.\n"
			     << "-h\t\t display this help information\n\n";
			exit(0);
		}
	}
	
	//seed the random number generator
	srand(time(NULL));
		
	//ogre init
	display = new OgreDisplay();
	display->setVerbose(true);
	display->initialize();
	
	if (display_axes)
	{
		display->addCube(Ogre::Vector3(0, 0, 0), Ogre::Vector3(0.25, 0.25, 0.25), Ogre::Quaternion::IDENTITY);
		display->addCube(Ogre::Vector3(0, 0, 0), Ogre::Vector3(0.1, 1000, 0.1), Ogre::Quaternion(Ogre::Radian(M_PI), Ogre::Vector3(1,1,0)));
		display->addCube(Ogre::Vector3(0, 0, 0), Ogre::Vector3(0.1, 1000, 0.1), Ogre::Quaternion(Ogre::Radian(M_PI), Ogre::Vector3(0,1,1)));
		display->addCube(Ogre::Vector3(0, 0, 0), Ogre::Vector3(0.1, 1000, 0.1), Ogre::Quaternion::IDENTITY);
	}

	//add a camera origin lights
	display->makeCameraLight(Ogre::ColourValue(0.4,0.4,0.4));
	display->makeOriginLight(Ogre::ColourValue(0.1,0.1,0.1));
	
	//voxel grid init
	vg = new VoxelGrid(voxel_grid_size);
	vg->setDisplay(display);
	vg->setUsePriorities(usePriorities);

	//set the VoxelGrid's output to verbose if the verbose flag was specified
	vg->setVerbose(verbose);
	vg->setUseCubes(useMarchingCubes);

	if (verbose)
	{
		std::cout << "Building voxel grid from file.\n";	
	}
	
	vg->setAdditionGranularity(grid_granularity);
	vg->readFromFile(interpreted_file);
	vg->getBoundingBoxes();

	if (scaleShapes)
	{
		if (verbose)
		{
			std::cout << "Scaling shapes into grid's boundaries.\n";
		}
		vg->scaleShapes();
	}
	
	vg->createShapes();

	if (verbose)
	{
		std::cout << "Doing the surface detail.\n";	
	}

	vg->setAutomataRuleSet(automata_rules_file);
	vg->setAutomataRuleMethod(automata_rules_method);
	vg->setAutomataIterations(automata_num_iterations);
	vg->setAutomataNeighbourhoodSize(automata_neighbourhood_size);
	vg->doSurfaceDetail();

	if (verbose)
	{
		std::cout << "Converting voxel grid to a mesh.\n";	
	}

	if (point_rendering)
	{
		vg->updateDisplay();	
	}
	else
	{
		vg->polygonize();	
	}

	//if the export mesh file name is not the empty string (i.e. we should export the created model to a mesh file after generating it.
	if (exported_mesh_file_name != "")
	{
		if (verbose)
		{
			std::cout << "Exporting the model to mesh file: " << exported_mesh_file_name << "\n";
		}
		
		//call the OgreDisplay method to do the exporting
		display->exportMeshToFile(exported_mesh_file_name);

		//clean up memory
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

		//exit the program
		exit(0);
	}
		
	if (drawBoundingBoxes)
	{
		//draw octree grid boundaries
		int grid_size = vg->getSize();
		display->addCube(Ogre::Vector3(grid_size, grid_size, grid_size / 2.0), Ogre::Vector3(0.25, 0.25, grid_size / 2.0), Ogre::Quaternion::IDENTITY);
		display->addCube(Ogre::Vector3(grid_size, grid_size / 2.0, grid_size), Ogre::Vector3(0.25, grid_size / 2.0, 0.25), Ogre::Quaternion::IDENTITY);
		display->addCube(Ogre::Vector3(grid_size / 2.0, grid_size, grid_size), Ogre::Vector3(grid_size / 2.0, 0.25, 0.25), Ogre::Quaternion::IDENTITY);

		//draw shapes bounding box
		Ogre::Vector3 min_corn = vg->getBoundingBoxMinCorner();		
		Ogre::Vector3 max_corn = vg->getBoundingBoxMaxCorner();
		Ogre::Vector3 box_size = max_corn - min_corn;
		Ogre::Vector3 mid_point = box_size * 0.5;

		// std::cout << min_corn.x << " " << min_corn.y << " " << min_corn.z << "\n"; //TEMP
		// std::cout << max_corn.x << " " << max_corn.y << " " << max_corn.z << "\n"; //TEMP
		// std::cout << box_size.x << " " << box_size.y << " " << box_size.z << "\n"; //TEMP
		// std::cout << mid_point.x << " " << mid_point.y << " " << mid_point.z << "\n"; //TEMP 
		
		// display->addCube(Ogre::Vector3(mid_point.x, max_corn.y, min_corn.z), Ogre::Vector3(box_size.x / 2.0, 0.25, 0.25), Ogre::Quaternion::IDENTITY);
		// display->addCube(Ogre::Vector3(mid_point.x, max_corn.y, max_corn.z), Ogre::Vector3(box_size.x / 2.0, 0.25, 0.25), Ogre::Quaternion::IDENTITY);
		// display->addCube(Ogre::Vector3(min_corn.x, max_corn.y, mid_point.z), Ogre::Vector3(0.25, 0.25, box_size.z / 2.0), Ogre::Quaternion::IDENTITY);
		// display->addCube(Ogre::Vector3(max_corn.x, max_corn.y, mid_point.z), Ogre::Vector3(0.25, 0.25, box_size.z / 2.0), Ogre::Quaternion::IDENTITY);
		
		// display->addCube(Ogre::Vector3(, , ), Ogre::Vector3(, , ), Ogre::Quaternion::IDENTITY);

		display->addCube(min_corn, Ogre::Vector3(1000, 0.25, 0.25), Ogre::Quaternion::IDENTITY);
		display->addCube(min_corn, Ogre::Vector3(0.25, 1000, 0.25), Ogre::Quaternion::IDENTITY);
		display->addCube(min_corn, Ogre::Vector3(0.25, 0.25, 1000), Ogre::Quaternion::IDENTITY);

		display->addCube(max_corn, Ogre::Vector3(1000, 0.25, 0.25), Ogre::Quaternion::IDENTITY);
		display->addCube(max_corn, Ogre::Vector3(0.25, 1000, 0.25), Ogre::Quaternion::IDENTITY);
		display->addCube(max_corn, Ogre::Vector3(0.25, 0.25, 1000), Ogre::Quaternion::IDENTITY);
	}

	if (verbose)
	{
		std::cout << "Setting up the OIS input system." << "\n";
	}
	
	//OIS init
	input = InputManager::getSingletonPtr();
	input->initialise(display->getRenderWindow());

	if (verbose)
	{
		std::cout << "Entering the main loop." << "\n";
	}

	//main game loop
	while (run)
	{
		tick();
		//draw frame
	}

	if (verbose)
	{
		std::cout << "Cleaning up memory.\n";	
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

	if (verbose)
	{
		cout << "\nEND OF PROGRAM\n";	
	}
	
	return 0;
}
