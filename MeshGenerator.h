/*
    Name: MeshGenerator.h
    
    Description:

    NOTE: The code to do the marching tetrahedra is based off Paul
    Bourke's code, which can be obtained at:

    http://paulbourke.net/geometry/polygonise/

    I take no credit for the marching cubes/tetrahedra part of this
    code, I only created the framework class around it and modified it
    as necessary for my purposes.

    */

#ifndef _MESH_GENERATOR_H_H
#define _MESH_GENERATOR_H_H

//standard library inclusions
#include "OGRE/Ogre.h"

//project inclusions
#include "Octree.h"
#include "OgreDisplay.h"


class MeshGenerator
{
private:
	//the tables that are used in the marching X algorithms
	static const float a2fVertexOffset[8][3];
	static const int a2iEdgeConnection[12][2];
	static const float a2fEdgeDirection[12][3];
	static const int a2iTetrahedronEdgeConnection[6][2];
	static const int a2iTetrahedronsInACube[6][4];
	static const int aiTetrahedronEdgeFlags[16];
	static const int a2iTetrahedronTriangles[16][7];
	static const int aiCubeEdgeFlags[256];
	static const int a2iTriangleConnectionTable[256][16];
	
	//threshold value which separates empty and solid space for the algorithm.
	float fTargetValue;
	//size of steps when doing the marching X algorithm
	float fStepSize;

	Octree* voxel_grid;

	OgreDisplay* display;

	//this is the manual object used for making meshes for the ships
	Ogre::ManualObject* ship_mesh;

	int mesh_vertex_count;

	bool verbose;

	float fGetOffset(float fValue1, float fValue2, float fValueDesired);

	Ogre::ColourValue vGetColor(Ogre::Vector3 &rfPosition, Ogre::Vector3 &rfNormal, std::set<std::string> in_tags = std::set<std::string>());

	void vMarchTetrahedron(Ogre::Vector3 *pasTetrahedronPosition, float *pafTetrahedronValue);

	Ogre::Vector3 vGetNormal(float fX, float fY, float fZ);

	float fSample(float fX, float fY, float fZ);

	Ogre::Vector3 vNormalizeVector(Ogre::Vector3 &rfVectorSource);

	void vMarchCube1(float fX, float fY, float fZ, float fScale);

	void vMarchCube2(float fX, float fY, float fZ, float fScale);

	void vMarch(bool useMarchingCubes);

public:
	MeshGenerator();

	~MeshGenerator();

	void vMarchingCubes();

	void vMarchingTetrahedrons();

	void setTargetValue(float in);
	
	float getTargetValue();
	
	void setStepSize(float in);
	
	float getStepSize();

	void setOctree(Octree* in);

	void setOgreDisplay(OgreDisplay* in);

	void setVerbose(bool in);
};

#endif
