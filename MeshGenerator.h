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

struct DetailingInformation
{
	Ogre::Vector3 position_offset;
	Ogre::Vector3 normal_offset;
	bool normalize_normals;
	Ogre::Vector3 normal_replacement;
	bool replace_normal;
	Ogre::ColourValue colour;
	bool set_colour;
	std::string material_name;

	DetailingInformation()
	{
		position_offset = Ogre::Vector3(0.0);
		normal_offset = Ogre::Vector3(0.0);
		normalize_normals = false;
		replace_normal = false;
		Ogre::Vector3 normal_replacement = Ogre::Vector3(0.0);
		colour = Ogre::ColourValue();
		set_colour = false;
		material_name = "";
	}
};

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

	bool onlyMarchSurfaceVoxels;

	//this is the manual object used for making meshes for the ships
	Ogre::ManualObject* ship_mesh;

	int mesh_vertex_count;

	bool verbose;

	bool do_detailing;

	//this is used in constraining depth when getting values from the octree. It's the biggest a node's size can be before we must stop recursing. A value of 1 means a leaf node, anything less than that is an aggregated node.
	int node_size_restriction;
	
	float fGetOffset(float fValue1, float fValue2, float fValueDesired);

	Ogre::ColourValue vGetColor(Ogre::Vector3 &rfPosition, Ogre::Vector3 &rfNormal, std::set<std::string> in_tags = std::set<std::string>());

	void vMarchTetrahedron(Ogre::Vector3 *pasTetrahedronPosition, float *pafTetrahedronValue, DetailingInformation* detail_info = NULL);

	Ogre::Vector3 vGetNormal(float fX, float fY, float fZ);

	float fSample(float fX, float fY, float fZ);

	Ogre::Vector3 vNormalizeVector(Ogre::Vector3 &rfVectorSource);

	void makeVertex(ManualObject* mesh, size_t in_index, Ogre::Vector3 in_pos, Ogre::Vector3 in_normal, Ogre::ColourValue in_colour, DetailingInformation* modifications);

	void vMarchCube1(float fX, float fY, float fZ, float fScale, DetailingInformation* detail_info = NULL);

	void vMarchCube2(float fX, float fY, float fZ, float fScale, DetailingInformation* detail_info = NULL);

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

	void setNodeSizeRestriction(int in);

	void setOnlyMarchSurfaceVoxels(bool in);
};

#endif
