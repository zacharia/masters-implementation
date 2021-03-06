/*

 */

#include "OgreDisplay.h"

//basic constructor. doesn't do anything
OgreDisplay::OgreDisplay()
{
	root = NULL;
	camera = NULL;
	sceneMgr = NULL;
	renderWindow = NULL;
	viewport = NULL;

	cubeCount = 0;
	cylinderCount = 0;
	lightCount = 0;

	cameraLight = NULL;
	originLight = NULL;

	verbose = false;

	model_name = "ship";
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
	//This code is used to stop ogre from spamming it's log to the command line.
	//It works by creating a dummy loglistener that doesn't output anything
	Ogre::LogManager* logger = new Ogre::LogManager();
	QuietLog qlog;
	logger->createLog("ogre/ogre.log", true, false, true);
	if (!verbose)
	{
		Ogre::LogManager::getSingleton().getDefaultLog()->addListener(&qlog);	
	}
		
	//use the files given 
	root = new Root("ogre/plugins.cfg", "ogre/ogre.cfg", "ogre/ogre.log");

	//this code chooses the renderer to use. without plugins.cfg, this won't work
	RenderSystemList* renderers = root->getAvailableRenderers();
	assert( !renderers->empty() );
	RenderSystem* renderSystems = renderers->at(0);
	root->setRenderSystem(renderSystems);

	//set up some options for our window
	root->getRenderSystem()->setConfigOption( "Full Screen", "No" );
	root->getRenderSystem()->setConfigOption( "Video Mode", "1920x1000" );
	root->getRenderSystem()->setConfigOption( "VSync", "Yes" );
	root->saveConfig();

	//FIXME: how can I get it to read this from resources.cfg instead of manually specifying it here?
	root->addResourceLocation("./content/", "FileSystem");
	
	//if (root->showConfigDialog())
	//FIXME: make it a command line thing to not show this?
	{
		renderWindow = root->initialise(true);
	}

	//choose scene manager, make camera and do clear colour
	sceneMgr = root->createSceneManager( ST_GENERIC, "PrototypeSceneManager");
	camera = sceneMgr->createCamera("ViewCamera");
	camera->setNearClipDistance(0.1);
	camera->setFarClipDistance(1000.0);
	
	viewport = renderWindow->addViewport(camera);
	viewport->setBackgroundColour(ColourValue(1, 1, 1));
	
	camera->setAspectRatio( Real(viewport->getActualWidth()) / Real(viewport->getActualHeight()) );

	//make a texture manager and resource groups
	TextureManager::getSingleton().setDefaultNumMipmaps(5);
	
	//ResourceGroupManager::getSingleton().createResourceGroup("procgen");
	//root->addResourceLocation("../content", "FileSystem", "procgen", true);
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	//some ambient light
	sceneMgr->setAmbientLight(ColourValue(0.0, 0.0, 0.0));
	//set the shadow type we'll be using - commented out because it was causing a crash
	//sceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

	//put the camera looking at the origins
	camera->setPosition(0,0,0);
	camera->lookAt(Vector3(64,64,64));

	//create a static geometry group for doing the voxels
	sceneMgr->createStaticGeometry("voxel_grid");

	//make a billboard group.
	sceneMgr->createBillboardSet("voxel_grid");
	sceneMgr->getRootSceneNode()->attachObject(sceneMgr->getBillboardSet("voxel_grid"));
	sceneMgr->getBillboardSet("voxel_grid")->setDefaultDimensions(1.0, 1.0);
	//enable this to rendering the voxels as points
	sceneMgr->getBillboardSet("voxel_grid")->setPointRenderingEnabled(true);
	sceneMgr->getBillboardSet("voxel_grid")->setMaterialName("basic/cube_default");

	//do mesh setup stuff
	cubeCount = 0;
	cylinderCount = 0;
	this->createCubeMesh(); //use default arguments to make the default cube
	this->createCylinderMesh(); //use default arguments to make the default cylinder
}


void OgreDisplay::createCubeMesh(std::string name, std::string material)
{
	//make a cube mesh
	
	ManualObject* cubeMesh = sceneMgr->createManualObject("cube");
	cubeMesh->begin(material, RenderOperation::OT_TRIANGLE_LIST);
	
	cubeMesh->position(-0.5, -0.5, -0.5); //0
	cubeMesh->normal(0,-1,0);
	cubeMesh->textureCoord(0,0);
	cubeMesh->position(-0.5, -0.5, 0.5); //1
	cubeMesh->normal(0,-1,0);
	cubeMesh->textureCoord(0,1);
	cubeMesh->position(0.5, -0.5, 0.5); //2
	cubeMesh->normal(0,-1,0);
	cubeMesh->textureCoord(1,1);
	cubeMesh->position(0.5, -0.5, -0.5); //3
	cubeMesh->normal(0,-1,0);
	cubeMesh->textureCoord(1,0);
	cubeMesh->position(-0.5, 0.5, -0.5); //4
	cubeMesh->normal(0,1,0);
	cubeMesh->textureCoord(0,0);
	cubeMesh->position(-0.5, 0.5, 0.5); //5
	cubeMesh->normal(0,1,0);
	cubeMesh->textureCoord(0,1);
	cubeMesh->position(0.5, 0.5, 0.5); //6
	cubeMesh->normal(0,1,0);
	cubeMesh->textureCoord(1,1);
	cubeMesh->position(0.5, 0.5, -0.5); //7
	cubeMesh->normal(0,1,0);
	cubeMesh->textureCoord(1,0);

	cubeMesh->position(-0.5, -0.5, -0.5); //8
	cubeMesh->normal(0,0,-1);
	cubeMesh->textureCoord(0,0);
	cubeMesh->position(-0.5, -0.5, 0.5); //9
	cubeMesh->normal(0,0,1);
	cubeMesh->textureCoord(0,0);
	cubeMesh->position(0.5, -0.5, 0.5); //10
	cubeMesh->normal(0,0,1);
	cubeMesh->textureCoord(1,0);
	cubeMesh->position(0.5, -0.5, -0.5); //11
	cubeMesh->normal(0,0,-1);
	cubeMesh->textureCoord(1,0);
	cubeMesh->position(-0.5, 0.5, -0.5); //12
	cubeMesh->normal(0,0,-1);
	cubeMesh->textureCoord(0,1);
	cubeMesh->position(-0.5, 0.5, 0.5); //13
	cubeMesh->normal(0,0,1);
	cubeMesh->textureCoord(0,1);
	cubeMesh->position(0.5, 0.5, 0.5); //14
	cubeMesh->normal(0,0,1);
	cubeMesh->textureCoord(1,1);
	cubeMesh->position(0.5, 0.5, -0.5); //15
	cubeMesh->normal(0,0,-1);
	cubeMesh->textureCoord(1,1);

	cubeMesh->position(-0.5, -0.5, -0.5); //16
	cubeMesh->normal(-1,0,0);
	cubeMesh->textureCoord(0,0);
	cubeMesh->position(-0.5, -0.5, 0.5); //17
	cubeMesh->normal(-1,0,0);
	cubeMesh->textureCoord(0,1);
	cubeMesh->position(0.5, -0.5, 0.5); //18
	cubeMesh->normal(1,0,0);
	cubeMesh->textureCoord(0,1);
	cubeMesh->position(0.5, -0.5, -0.5); //19
	cubeMesh->normal(1,0,0);
	cubeMesh->textureCoord(0,0);
	cubeMesh->position(-0.5, 0.5, -0.5); //20
	cubeMesh->normal(-1,0,0);
	cubeMesh->textureCoord(1,0);
	cubeMesh->position(-0.5, 0.5, 0.5); //21
	cubeMesh->normal(-1,0,0);
	cubeMesh->textureCoord(1,1);
	cubeMesh->position(0.5, 0.5, 0.5); //22
	cubeMesh->normal(1,0,0);
	cubeMesh->textureCoord(1,1);
	cubeMesh->position(0.5, 0.5, -0.5); //23
	cubeMesh->normal(1,0,0);
	cubeMesh->textureCoord(1,0);
	
	//bottom
	cubeMesh->quad(3,2,1,0);
	//top
	cubeMesh->quad(4,5,6,7);
	//front
	cubeMesh->quad(10,14,13,9);
	//back
	cubeMesh->quad(8,12,15,11);
	//left
	cubeMesh->quad(16,17,21,20);
	//right
	cubeMesh->quad(23,22,18,19);

	cubeMesh->end();

	cubeMesh->convertToMesh(name);
}


void OgreDisplay::createCylinderMesh(std::string name, std::string material)
{
	//make a cube mesh
	
	ManualObject* cylinderMesh = sceneMgr->createManualObject("cylinder");
	cylinderMesh->begin(material, RenderOperation::OT_TRIANGLE_LIST);
	
	for(double i = 0.0; i < Math::PI * 2; i += Math::PI / 5)
	{
		cylinderMesh->position(cos(i), 1, sin(i));
		cylinderMesh->textureCoord(i / (Math::PI * 2), 1.0);
		Ogre::Vector3 myNormal(cos(i), 0, sin(i));
		myNormal.normalise();
		cylinderMesh->normal(myNormal);
	}
	
	for(double i = 0.0; i < Math::PI * 2; i += Math::PI / 5)
	{
		cylinderMesh->position(cos(i), -1, sin(i));
		cylinderMesh->textureCoord(i / (Math::PI * 2), 0.0);
		Ogre::Vector3 myNormal(cos(i), 0, sin(i));
		myNormal.normalise();
		cylinderMesh->normal(myNormal);
	}
	
	for(int i = 0; i < 10; i++)
	{
		cylinderMesh->triangle(i, (i+1) % 10, ((i + 10) % 20));
	}
	
	for(int i = 10; i < 20; i++)
	{
		cylinderMesh->triangle(i, (i+1) % 10, (i+1) % 10 + 10);
	}
	
	cylinderMesh->position(0, 1, 0);
	cylinderMesh->textureCoord(0.5, 0.5);
	
	cylinderMesh->position(0, -1, 0);
	cylinderMesh->textureCoord(0.5, 0.5);
	
	for(int i = 0; i < 10; i++)
	{
		cylinderMesh->triangle(20, (i+1) % 10, i);
		cylinderMesh->triangle(21, ((i+10) % 10) + 10, ((i+ 11) % 10) + 10);
	}

	cylinderMesh->end();

	cylinderMesh->convertToMesh(name);
}


//this is used to let OIS get the screen to track
RenderWindow* OgreDisplay::getRenderWindow()
{
	return renderWindow;
}


//draw one frame
void OgreDisplay::drawFrame()
{
	if (cameraLight != NULL)
	{
		cameraLight->setPosition(camera->getPosition());
	}
	
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
	std::string name = "v" + Utility::numToString(pos.x) + " " + Utility::numToString(pos.y) + " " + Utility::numToString(pos.z);
	SceneNode* newVoxel = sceneMgr->getRootSceneNode()->createChildSceneNode();
	newVoxel->attachObject(sceneMgr->createEntity(name, /*"voxel"*/ "defaultCubeMesh"));
	newVoxel->translate(pos.x, pos.y, pos.z);
}


//this method adds a voxel into the world as static geometry
void OgreDisplay::addVoxelStatic(Vector3 pos)
{
	//make the voxel cube normally, and attach it to a scene node
	std::string name = "v" + Utility::numToString(pos.x) + " " + Utility::numToString(pos.y) + " " + Utility::numToString(pos.z);
	SceneNode* newVoxel = sceneMgr->getRootSceneNode()->createChildSceneNode();
	newVoxel->attachObject(sceneMgr->createEntity(name, /*"voxel"*/ "defaultCubeMesh"));
	newVoxel->translate(pos.x, pos.y, pos.z);

	//add the scene node to the static geometry
	sceneMgr->getStaticGeometry("voxel_grid")->addSceneNode(newVoxel);
}


void OgreDisplay::buildStaticGeometry()
{
	( sceneMgr->getStaticGeometry("voxel_grid") )->build();
}


Camera* OgreDisplay::getCamera()
{
	return camera;
}


void OgreDisplay::addVoxelBillboard(Vector3 pos)
{
	sceneMgr->getBillboardSet("voxel_grid")->createBillboard(Vector3(pos.x, pos.y, pos.z));
}


/*
  Adds a rectangle to the scene.
  pos - it's position in space (centre)
  scale - it's dimensions (extents to the corner of bounding box from the centre)
  rot - a quaternion that gives it's orientation.
  materialName - the material to use for this rectangle
 */
void OgreDisplay::addCube(Vector3 pos, Vector3 scale, Quaternion rot, std::string materialName, bool randCol)
{
	SceneNode* node = sceneMgr->getRootSceneNode()->createChildSceneNode(pos, rot);
	node->setScale(scale * 2.0);

	Entity* ent = sceneMgr->createEntity("cube" + Utility::numToString(cubeCount), "defaultCubeMesh");
	cubeCount++;

	if (randCol)
	{
		MaterialPtr tempMat = ent->getSubEntity(0)->getMaterial()->clone("randCubeCol" + Utility::numToString(cubeCount));
		tempMat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setColourOperationEx(LBX_SOURCE1, LBS_MANUAL, LBS_CURRENT, ColourValue(Math::RangeRandom(0.0, 1.0), Math::RangeRandom(0.0, 1.0), Math::RangeRandom(0.0, 1.0), 1.0));
		
		ent->setMaterialName("randCubeCol" + Utility::numToString(cubeCount));
	}
	else
	{
		ent->setMaterialName(materialName);
	}
	
	node->attachObject(ent);
}


void OgreDisplay::addCylinder(Vector3 pos, Vector3 scale, Quaternion rot, std::string materialName, bool randCol)
{
	SceneNode* node = sceneMgr->getRootSceneNode()->createChildSceneNode(pos, rot);
	node->setScale(scale * 2.0);

	Entity* ent = sceneMgr->createEntity("cylinder" + Utility::numToString(cylinderCount), "defaultCylinderMesh");
	cylinderCount++;

	if (randCol)
	{
		MaterialPtr tempMat = ent->getSubEntity(0)->getMaterial()->clone("randCylinderCol" + Utility::numToString(cylinderCount));
		tempMat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setColourOperationEx(LBX_SOURCE1, LBS_MANUAL, LBS_CURRENT, ColourValue(Math::RangeRandom(0.0, 1.0), Math::RangeRandom(0.0, 1.0), Math::RangeRandom(0.0, 1.0), 1.0));
		
		ent->setMaterialName("randCylinderCol" + Utility::numToString(cylinderCount));
	}
	else
	{
		ent->setMaterialName(materialName);
	}

	node->attachObject(ent);
}


ManualObject* OgreDisplay::createManualObject(std::string name)
{
	return sceneMgr->createManualObject(name);
}


SceneManager* OgreDisplay::getSceneManager()
{
	return sceneMgr;
}


/*
  this method creates a light in the scene, based on the arguments given.
 */
Light* OgreDisplay::createLight(std::string type, Vector3 pos, ColourValue col, std::string lname)
{
	Light* light;
	
	//FIXME: this only does point lights at the moment
	if (lname != "")
	{
		light = sceneMgr->createLight(lname);
	}
	else
	{
		light = sceneMgr->createLight( "light" + Utility::numToString(lightCount) );
		lightCount++;
	}
	
	if (type == "point")
	{
		light->setType(Light::LT_POINT);
	}
	//default to a point light
	else
	{
		light->setType(Light::LT_POINT);
	}
	
	light->setPosition(pos);
	light->setDiffuseColour(col);
	light->setSpecularColour(col);

	return light;
}


/*
  this method sets up a light that is centred at the camera.
  if enable is true, the light is created/turned on again.
  if enable is false, the light is deactivated
 */
void OgreDisplay::makeCameraLight(ColourValue col)
{	
	if (cameraLight == NULL)
	{
		cameraLight = createLight("point", camera->getPosition(), col, "camera_light");
	}
	else
	{
		cameraLight->setDiffuseColour(col);
		cameraLight->setSpecularColour(col);
	}	
}


/*
  this method sets up a light at the origin.
  if enable is true, the light is created/turned on again.
  if enable is false, the light is deactivated
*/
void OgreDisplay::makeOriginLight(ColourValue col)
{
	if (originLight == NULL)
	{
		originLight = createLight("point", Vector3(0,0,0), col, "origin_light");
	}
	else
	{
		originLight->setDiffuseColour(col);
		originLight->setSpecularColour(col);
	}
}


void OgreDisplay::toggleCameraLight()
{
	if (cameraLight != NULL)
	{
		sceneMgr->destroyLight("camera_light");
		cameraLight = NULL;
	}
	else
	{
		makeCameraLight(ColourValue(0,0.7,0.0));
	}
}


void OgreDisplay::toggleOriginLight()
{
	if (originLight != NULL)
	{
		sceneMgr->destroyLight("origin_light");
		originLight = NULL;
	}
	else
	{
		makeOriginLight(ColourValue(0.7,0,0));
	}
}


void OgreDisplay::QuietLog::messageLogged(const String& message, LogMessageLevel lml, bool maskDebug, const String& logName)
{
	switch (lml)
	{
	case LML_NORMAL:
		break;
	case LML_CRITICAL:
		break;
	case LML_TRIVIAL:
		break;
	}
}


void OgreDisplay::setVerbose(bool v)
{
	verbose = v;
}


void OgreDisplay::exportMeshToFile(std::string file_name)
{
	Ogre::MeshPtr model_pointer = sceneMgr->getEntity(model_name)->getMesh();
	if (model_pointer.isNull())
	{
		std::cout << "ERROR: could not export model to file.";
	}
	else
	{
		Ogre::MeshSerializer model_exporter;
		model_exporter.exportMesh(model_pointer.getPointer(), file_name);
	}
}


std::string OgreDisplay::getModelName()
{
	return model_name;
}


void OgreDisplay::takeScreenshot(std::string file_name)
{
	this->renderWindow->writeContentsToFile(file_name);
}
