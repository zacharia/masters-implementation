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

	std::string intToString(int in);
	
public:
	OgreDisplay();

	~OgreDisplay();
	
	void initialize();

	void createPrimitiveMeshes();

	RenderWindow* getRenderWindow();

	void drawFrame();

	void addVoxel(Vector3 pos);

	void addVoxelStatic(Vector3 pos);

	void addVoxelBillboard(Vector3 pos);

	void createVoxelMesh();

	void buildStaticGeometry();

	Camera* getCamera();
};

#endif
