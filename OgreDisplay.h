/* ---------------------------------------------------------------------------

** -------------------------------------------------------------------------*/

#ifndef _OGRE_DISPLAY_H
#define _OGRE_DISPLAY_H

#include <OGRE/Ogre.h>
#include <sstream>

//project inclusions
#include "Utility.h"
#include "TriangleMesh.h"

using namespace Ogre;

class OgreDisplay
{
	Root* root;
	Camera* camera;
	SceneManager* sceneMgr;
	RenderWindow* renderWindow;
	Viewport* viewport;

	int cubeCount;
	int cylinderCount;
	int lightCount;

	//if this is non-null, there is a light named "camera_light" that should be moved to the camera position
	//at each frame
	Light* cameraLight;	
	//if this is non-null then it is a light that should be placed at the origin.
	Light* originLight;

	//This is used as a dummy class to stop ogre from spamming it's log to the command line.
	class QuietLog : public Ogre::LogListener
	{
		void messageLogged(const String& message, LogMessageLevel lml, bool maskDebug, const String& logName);
	};
		
public:
	OgreDisplay();

	~OgreDisplay();
	
	void initialize();

	RenderWindow* getRenderWindow();

	void drawFrame();

	void addVoxel(Vector3 pos);

	void addVoxelStatic(Vector3 pos);

	void addVoxelBillboard(Vector3 pos);

	void addCube(Vector3 pos, Vector3 scale = Vector3(1,1,1), Quaternion rot = Quaternion::IDENTITY, std::string materialName = "basic/cube_default", bool randCol = true);

	void addCylinder(Vector3 pos, Vector3 scale = Vector3(1,1,1), Quaternion rot = Quaternion::IDENTITY, std::string materialName = "basic/cylinder_default", bool randCol = true);

	void createVoxelMesh();

	void buildStaticGeometry();

	void createCubeMesh(std::string name = "defaultCubeMesh", std::string material = "basic/cube_default");

	void createCylinderMesh(std::string name = "defaultCylinderMesh", std::string material = "basic/cylinder_default");

	Camera* getCamera();

	ManualObject* createManualObject(std::string name);

	SceneManager* getSceneManager();

	Light* createLight(std::string type, Vector3 pos, ColourValue col, std::string lname = "");

	void makeCameraLight(ColourValue col = ColourValue(1,1,1));

	void makeOriginLight(ColourValue col = ColourValue(1,1,1));

	void toggleCameraLight();

	void toggleOriginLight();

	void createTriangleMesh(std::string name, std::string material = "basic/backface_culling_off");

	void addToTriangleMesh(std::string name, TriangleMesh* input_mesh, bool finish = false);
};

#endif
