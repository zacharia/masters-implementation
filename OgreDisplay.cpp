/*

 */

#include "OgreDisplay.h"

//basic constructor. doesn't do anything
OgreDisplay::OgreDisplay()
{
	root = NULL;
}

//deletes root.
OgreDisplay::~OgreDisplay()
{
	if (root != NULL)
	{
		delete root;
	}
}

//does all the initial setup of the window, camera, etc.
void OgreDisplay::initialize()
{
	//use the files given 
	root = new Root("ogre/plugins.cfg", "ogre/ogre.cfg", "ogre/ogre.log");

	//this code chooses the renderer to use. without plugins.cfg, this won't work
	RenderSystemList* renderers = root->getAvailableRenderers();
	assert( !renderers->empty() );
	RenderSystem* renderSystems = renderers->at(0);
	root->setRenderSystem(renderSystems);

	//set up some options for our window
	root->getRenderSystem()->setConfigOption( "Full Screen", "No" );
	root->getRenderSystem()->setConfigOption( "Video Mode", "1400x1050" );
	root->getRenderSystem()->setConfigOption( "VSync", "Yes" );
	root->saveConfig();
	
	//if (root->showConfigDialog())
	//FIXME: make it a command line thing to not show this?
	{
		renderWindow = root->initialise(true);
	}

	//choose scene manager, make camera and do clear colour
	sceneMgr = root->createSceneManager( ST_GENERIC, "PrototypeSceneManager");
	camera = sceneMgr->createCamera("ViewCamera");
	camera->setNearClipDistance(0.1);
	camera->setFarClipDistance(200.0);
	
	viewport = renderWindow->addViewport(camera);
	viewport->setBackgroundColour(ColourValue(0, 0, 0));
	
	camera->setAspectRatio( Real(viewport->getActualWidth()) / Real(viewport->getActualHeight()) );

	//make a texture manager and resource groups
	TextureManager::getSingleton().setDefaultNumMipmaps(5);
	
	//ResourceGroupManager::getSingleton().createResourceGroup("procgen");
	//root->addResourceLocation("../content", "FileSystem", "procgen", true);
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	//some ambient light
	sceneMgr->setAmbientLight(ColourValue(1, 1, 1));

	//put the camera looking at the origins
	camera->setPosition(0,5,5);
	camera->lookAt(Vector3(0,0,0));

	//create a static geometry group for doing the voxels
	sceneMgr->createStaticGeometry("voxel_grid");

	//make a billboard group.
	sceneMgr->createBillboardSet("voxel_grid");
	sceneMgr->getRootSceneNode()->attachObject(sceneMgr->getBillboardSet("voxel_grid"));
	sceneMgr->getBillboardSet("voxel_grid")->setDefaultDimensions(1.0, 1.0);
	//enable this to rendering the voxels as points
	sceneMgr->getBillboardSet("voxel_grid")->setPointRenderingEnabled(true);
}

//this is used to let OIS get the screen to track
RenderWindow* OgreDisplay::getRenderWindow()
{
	return renderWindow;
}

//draw one frame
void OgreDisplay::drawFrame()
{
	root->renderOneFrame();
	renderWindow->update();
}

//this makes the cube mesh used to represent a voxel.
void OgreDisplay::createVoxelMesh()
{
	ManualObject* manual = sceneMgr->createManualObject("voxel");
	manual->begin("BaseWhite", RenderOperation::OT_TRIANGLE_LIST);

	manual->position(-0.5, -0.5, -0.5); //0
	manual->position(-0.5, -0.5, 0.5); //1
	manual->position(0.5, -0.5, 0.5); //2
	manual->position(0.5, -0.5, -0.5); //3
	manual->position(-0.5, 0.5, -0.5); //4
	manual->position(-0.5, 0.5, 0.5); //5
	manual->position(0.5, 0.5, 0.5); //6
	manual->position(0.5, 0.5, -0.5); //7

	manual->quad(0,1,2,3);
	manual->quad(4,5,6,7);
	manual->quad(0,1,4,5);
	manual->quad(2,3,6,7);
	manual->quad(1,2,5,6);
	manual->quad(0,3,4,7);

	manual->end();

	manual->convertToMesh("voxel");
}

//this method adds a voxel into the world as an entity
void OgreDisplay::addVoxel(Vector3 pos)
{
	//make the voxel cube normally, and attach it to a scene node
	std::string name = "v" + intToString(pos.x) + " " + intToString(pos.y) + " " + intToString(pos.z);
	SceneNode* newVoxel = sceneMgr->getRootSceneNode()->createChildSceneNode();
	newVoxel->attachObject(sceneMgr->createEntity(name, "voxel"));
	newVoxel->translate(pos.x, pos.y, pos.z);
}

//this method adds a voxel into the world as static geometry
void OgreDisplay::addVoxelStatic(Vector3 pos)
{
	//make the voxel cube normally, and attach it to a scene node
	std::string name = "v" + intToString(pos.x) + " " + intToString(pos.y) + " " + intToString(pos.z);
	SceneNode* newVoxel = sceneMgr->getRootSceneNode()->createChildSceneNode();
	newVoxel->attachObject(sceneMgr->createEntity(name, "voxel"));
	newVoxel->translate(pos.x, pos.y, pos.z);

	//add the scene node to the static geometry
	sceneMgr->getStaticGeometry("voxel_grid")->addSceneNode(newVoxel);
}

void OgreDisplay::buildStaticGeometry()
{
	( sceneMgr->getStaticGeometry("voxel_grid") )->build();
}

std::string OgreDisplay::intToString(int in)
{
	std::stringstream convert;
	convert << in;
	return convert.str();
}

Camera* OgreDisplay::getCamera()
{
	return camera;
}

void OgreDisplay::addVoxelBillboard(Vector3 pos)
{
	sceneMgr->getBillboardSet("voxel_grid")->createBillboard(Vector3(pos.x, pos.y, pos.z));
}
