/* ---------------------------------------------------------------------------

** -------------------------------------------------------------------------*/

#ifndef _OGRE_DISPLAY_H
#define _OGRE_DISPLAY_H

#include <OGRE/Ogre.h>
#include <sstream>

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

	std::string intToString(int in);
	
public:
	OgreDisplay();

	~OgreDisplay();
	
	void initialize();

	RenderWindow* getRenderWindow();

	void drawFrame();

	void addVoxel(Vector3 pos);

	void addVoxelStatic(Vector3 pos);

	void addVoxelBillboard(Vector3 pos);

	void addCube(Vector3 pos, Vector3 scale = Vector3(1,1,1), Quaternion rot = Quaternion::IDENTITY, std::string meshName = "defaultCubeMesh");

	void addCylinder(Vector3 pos, Vector3 scale = Vector3(1,1,1), Quaternion rot = Quaternion::IDENTITY, std::string meshName = "defaultCylinderMesh");

	void createVoxelMesh();

	void buildStaticGeometry();

	void createCubeMesh(std::string name = "defaultCubeMesh", std::string material = "basic/cube_default");

	void createCylinderMesh(std::string name = "defaultCylinderMesh", std::string material = "basic/cylinder_default");

	Camera* getCamera();
};

#endif
