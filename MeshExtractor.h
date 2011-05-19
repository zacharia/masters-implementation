

/*
 * Adapted from:
 * marching_cubes.h
 *  MC Algorithm implemented by Paul Bourke wiki contribution
 *  Qt-Adaption Created on: 15.07.2009
 *      Author: manitoo
 *  Available: http://local.wasp.uwa.edu.au/~pbourke/geometry/polygonise/index.html
 */

#ifndef GEOMETRYEXTRACTOR_H_
#define GEOMETRYEXTRACTOR_H_

#include <cmath>
/* #include <QVector3D> */

/* #include "GlobalDefines.h" */
#include "TriangleMesh.h"

#include "VoxelGrid.h" //ADDED BY ME
#include <OGRE/Ogre.h> //ADDED BY ME

class TriangleMesh;

/* ---------- Class to compute & display everything ----------- */
class MeshExtractor
{

public:
	MeshExtractor()
	{
		width = 0;
		height = 0;
		depth = 0;

		octree = NULL; //ADDED BY ME
		verbose = false; //ADDED BY ME
	};
    ~MeshExtractor() {
    	width = 0;
		height = 0;
		depth = 0;
    };


    // there is no float data, int target method because in this case float, float would be better suited

    /**
     * Use Marching Cubes to generate a Triangle Mesh from an input data array
     *
     * This implementation of marching cubes assumes the OpenGL coordinate system: y is
     * positive in the upwards direction, and x & z are on the horizontal plane.
     *
     * @param	data		1D array of data from which to generate the mesh. This data is
     * 						not editted and should be packed in the standard 3D layout:
     * 						(x,y,z) = [z][y][x] = [x + y*width + z*with*height]
     *
     * @param	widthIn 	The width (x dimension) of your 3D data structure
     * @param	heightIn	The height (y dimension) of your 3D data structure
     * @param	depthIn		The depth (z dimension) of your 3D data structure
     *
     * @param	targetValue	The value in your grid that should be interpretted as the
     * 						boundary between solid and empty
     *
     * @param	mesh		The pointer to the TriangleMesh object that you wish to
     * 						populate with triangles representing your input array in 3D
     */
    void extractMeshWithMarchingCubes (
			const float* data,
			const int &widthIn,
			const int &heightIn,
			const int &depthIn,
			const float targetValue,
			TriangleMesh* mesh)
    {
    	marchingCubes<float, float> (data,widthIn,heightIn,depthIn,targetValue,mesh);
    }

    void extractMeshWithMarchingCubes (
    		const int* data,
			const int &widthIn,
			const int &heightIn,
			const int &depthIn,
			const float targetValue,
			TriangleMesh* mesh)
    {
    	marchingCubes<int, float> (data,widthIn,heightIn,depthIn,targetValue,mesh);
    }

    void extractMeshWithMarchingCubes (
			const int* data,
			const int &widthIn,
			const int &heightIn,
			const int &depthIn,
			const int targetValue,
			TriangleMesh* mesh)
    {
    	marchingCubes<int, int> (data,widthIn,heightIn,depthIn,targetValue,mesh);
    }


    void extractMeshWithMarchingTetrahedra(
			const int* data,
			const int &widthIn,
			const int &heightIn,
			const int &depthIn,
			const int targetValue,
			TriangleMesh* mesh)
    {
    	marchingTetrahedra<int, int> (data,widthIn,heightIn,depthIn,targetValue,mesh);
    }

    void extractMeshWithMarchingTetrahedra(
			const int* data,
			const int &widthIn,
			const int &heightIn,
			const int &depthIn,
			const float targetValue,
			TriangleMesh* mesh)
    {
    	marchingTetrahedra<int, float> (data,widthIn,heightIn,depthIn,targetValue,mesh);
    }

    void extractMeshWithMarchingTetrahedra(
			const float* data,
			const int &widthIn,
			const int &heightIn,
			const int &depthIn,
			const float targetValue,
			TriangleMesh* mesh)
    {
    	marchingTetrahedra<float, float> (data,widthIn,heightIn,depthIn,targetValue,mesh);
    }

    template <typename DATA_TYPE, typename TARGET_TYPE> void marchingCubes(
			const DATA_TYPE* data,
			const int &widthIn,
			const int &heightIn,
			const int &depthIn,
			const TARGET_TYPE targetValue,
			TriangleMesh* mesh)
    {
    	width = widthIn;
    	height = heightIn;
    	depth = depthIn;

        //Start Computation
    	marchCube1<DATA_TYPE,TARGET_TYPE>(data, targetValue, mesh);

    };

    template <typename DATA_TYPE, typename TARGET_TYPE> void marchingTetrahedra(
    		const DATA_TYPE* data,
			const int &widthIn,
			const int &heightIn,
			const int &depthIn,
			const TARGET_TYPE targetValue,
			TriangleMesh* mesh)
    {
    	width = widthIn;
    	height = heightIn;
    	depth = depthIn;
        //Start Computation

    	marchCube2<DATA_TYPE,TARGET_TYPE>(data, targetValue, mesh);

    };

    //ADDED BY ME
    void setOctree(Octree* in)
    {
	    octree = in;
    }
    
    //ADDED BY ME
    void setVerbose(bool v)
    {
	    verbose = v;
    }

private:

    int width;
    int height;
    int depth;

    Octree* octree; // ADDED BY ME
    bool verbose; //ADDED BY ME

    /**********     Marching Cubes Methods       ************/
    //marchCube1 performs the Marching Cubes algorithm on a single cube
    template <typename DATA_TYPE, typename TARGET_TYPE> void marchCube1(
    		const DATA_TYPE* data,
			const TARGET_TYPE &targetValue,
			TriangleMesh* mesh)
    {
		for(int x = 0; x< width; ++x)
		{
			//ADDED BY ME (if block)
			if (verbose)
			{
				std::cout << "doing slice " << x << " of " << width << "\n";
			}
			for(int y = 0; y < height; ++y)
			{
				for(int z = 0; z < depth; ++z)
				{
					int iVertex, vertex0, vertex1, vertex2, iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
					float fOffset;
					Vec3f sColor;
					DATA_TYPE afCubeValue[8];
					Vec3f asEdgeVertex[12];
					Vec3f asEdgeNorm[12];

					//Make a local copy of the values at the cube's corners
					for(iVertex = 0; iVertex < 8; ++iVertex)
					{
						afCubeValue[iVertex] = getDiscreetValue<DATA_TYPE>(data, x + a2fVertexOffset[iVertex][0],y + a2fVertexOffset[iVertex][1],z + a2fVertexOffset[iVertex][2]);
					}

					//Find which vertices are inside of the surface and which are outside
					iFlagIndex = 0;
					for(iVertexTest = 0; iVertexTest < 8; ++iVertexTest)
					{
							if(afCubeValue[iVertexTest] <= targetValue)     iFlagIndex |= 1<<iVertexTest;
					}

					//Find which edges are intersected by the surface
					iEdgeFlags = aiCubeEdgeFlags[iFlagIndex];

					//If the cube is entirely inside or outside of the surface, then there will be no intersections
					if(iEdgeFlags == 0)
					{
							continue;
					}

					//Find the point of intersection of the surface with each edge
					//Then find the normal to the surface at those points
					for(iEdge = 0; iEdge < 12; ++iEdge)
					{
						//if there is an intersection on this edge
						if(iEdgeFlags & (1<<iEdge))
						{
							fOffset = getOffset<DATA_TYPE, TARGET_TYPE>(afCubeValue[ a2iEdgeConnection[iEdge][0] ],afCubeValue[ a2iEdgeConnection[iEdge][1] ], targetValue);

							asEdgeVertex[iEdge].x =  x + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][0]  +  fOffset * a2fEdgeDirection[iEdge][0]);
							asEdgeVertex[iEdge].y =  y + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][1]  +  fOffset * a2fEdgeDirection[iEdge][1]);
							asEdgeVertex[iEdge].z =  z + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][2]  +  fOffset * a2fEdgeDirection[iEdge][2]);

							getNormal<DATA_TYPE>(asEdgeNorm[iEdge], asEdgeVertex[iEdge].x, asEdgeVertex[iEdge].y, asEdgeVertex[iEdge].z,data);
						}
					}


						//Draw the triangles that were found.  There can be up to five per cube
					for(iTriangle = 0; iTriangle < 5; ++iTriangle)
					{
						if(a2iTriangleConnectionTable[iFlagIndex][3*iTriangle] < 0) break;

						vertex0 = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle];
						vertex1 = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+1];
						vertex2 = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+2];
						mesh->addTriangle(asEdgeVertex[vertex0],asEdgeVertex[vertex1],asEdgeVertex[vertex2],asEdgeNorm[vertex0],asEdgeNorm[vertex1],asEdgeNorm[vertex2]);
					}
				}
			}
		}
    };

    //marchCube2 performs the Marching Tetrahedrons algorithm on a single cube by making six calls to marchTetrahedron
    template <typename DATA_TYPE, typename TARGET_TYPE> void marchCube2(
    		const DATA_TYPE* data,
			const TARGET_TYPE &targetValue,
			TriangleMesh* mesh)
    {
    	for(int x = 0; x< width; ++x)
    	{
		//ADDED BY ME (if block)
		if (verbose)
		{
			std::cout << "doing slice " << x << " of " << width << "\n";
		}
    		for(int y = 0; y < height; ++y)
    		{
    			for(int z = 0; z < depth; ++z)
				{
					int iVertex, iTetrahedron, iVertexInACube;
					Vec3f asCubePosition[8];
					DATA_TYPE  afCubeValue[8];
					Vec3f asTetrahedronPosition[4];
					DATA_TYPE  afTetrahedronValue[4];

					//Make a local copy of the cube's corner positions
					for(iVertex = 0; iVertex < 8; ++iVertex)
					{
							asCubePosition[iVertex].x =  x + a2fVertexOffset[iVertex][0];
							asCubePosition[iVertex].y =  y + a2fVertexOffset[iVertex][1];
							asCubePosition[iVertex].z =  z + a2fVertexOffset[iVertex][2];
					}

					//Make a local copy of the cube's corner values
					for(iVertex = 0; iVertex < 8; ++iVertex)
					{
						afCubeValue[iVertex] = getDiscreetValue<DATA_TYPE>(data, asCubePosition[iVertex].x,asCubePosition[iVertex].y,asCubePosition[iVertex].z);
					}

					for(iTetrahedron = 0; iTetrahedron < 6; ++iTetrahedron)
					{
						for(iVertex = 0; iVertex < 4; ++iVertex)
						{
							iVertexInACube = a2iTetrahedronsInACube[iTetrahedron][iVertex];
							asTetrahedronPosition[iVertex].x =  asCubePosition[iVertexInACube].x;
							asTetrahedronPosition[iVertex].y =  asCubePosition[iVertexInACube].y;
							asTetrahedronPosition[iVertex].z =  asCubePosition[iVertexInACube].z;
							afTetrahedronValue[iVertex] = afCubeValue[iVertexInACube];
						}
						marchTetrahedron<DATA_TYPE,TARGET_TYPE>(data, asTetrahedronPosition, afTetrahedronValue, targetValue, mesh);
					}
				}
    		}
    	}
    };

    //marchTetrahedron performs the Marching Tetrahedrons algorithm on a single tetrahedron
    template <typename DATA_TYPE, typename TARGET_TYPE> void marchTetrahedron(
    		const DATA_TYPE* data,
    		Vec3f *pasTetrahedronPosition,
    		DATA_TYPE *pafTetrahedronValue,
    		const TARGET_TYPE &targetValue,
			TriangleMesh* mesh)
    {
    	int iEdge, iVert0, iVert1, iEdgeFlags, iTriangle, iVertex, vertex0, vertex1, vertex2, iFlagIndex = 0;
    	float fOffset, fInvOffset;
    	Vec3f asEdgeVertex[6];
    	Vec3f asEdgeNorm[6];
    	Vec3f sColor;

    	//Find which vertices are inside of the surface and which are outside
    	for(iVertex = 0; iVertex < 4; ++iVertex)
    	{
    			if(pafTetrahedronValue[iVertex] <= targetValue)     iFlagIndex |= 1<<iVertex;
    	}

    	//Find which edges are intersected by the surface
    	iEdgeFlags = aiTetrahedronEdgeFlags[iFlagIndex];

    	//If the tetrahedron is entirely inside or outside of the surface, then there will be no intersections
    	if(iEdgeFlags == 0)
    	{
    			return;
    	}
    	//Find the point of intersection of the surface with each edge
    	// Then find the normal to the surface at those points
    	for(iEdge = 0; iEdge < 6; ++iEdge)
    	{
    		//if there is an intersection on this edge
    		if(iEdgeFlags & (1<<iEdge))
    		{
    			iVert0 = a2iTetrahedronEdgeConnection[iEdge][0];
    			iVert1 = a2iTetrahedronEdgeConnection[iEdge][1];
    			fOffset = getOffset<TARGET_TYPE>(pafTetrahedronValue[iVert0], pafTetrahedronValue[iVert1], targetValue);
    			fInvOffset = 1.0 - fOffset;

    			asEdgeVertex[iEdge].x =  fInvOffset*pasTetrahedronPosition[iVert0].x  +  fOffset*pasTetrahedronPosition[iVert1].x;
    			asEdgeVertex[iEdge].y =  fInvOffset*pasTetrahedronPosition[iVert0].y  +  fOffset*pasTetrahedronPosition[iVert1].y;
    			asEdgeVertex[iEdge].z =  fInvOffset*pasTetrahedronPosition[iVert0].z  +  fOffset*pasTetrahedronPosition[iVert1].z;

    			getNormal<DATA_TYPE>(asEdgeNorm[iEdge], rintf(asEdgeVertex[iEdge].x), rintf(asEdgeVertex[iEdge].y), rintf(asEdgeVertex[iEdge].z),data);
    		}
    	}
    	//Draw the triangles that were found.  There can be up to 2 per tetrahedron
    	for(iTriangle = 0; iTriangle < 2; ++iTriangle)
    	{
    		if(a2iTetrahedronTriangles[iFlagIndex][3*iTriangle] < 0)    break;

    		vertex0 = a2iTetrahedronTriangles[iFlagIndex][3*iTriangle];
    		vertex1 = a2iTetrahedronTriangles[iFlagIndex][3*iTriangle+1];
    		vertex2 = a2iTetrahedronTriangles[iFlagIndex][3*iTriangle+2];

    		mesh->addTriangle(asEdgeVertex[vertex0],asEdgeVertex[vertex1],asEdgeVertex[vertex2],asEdgeNorm[vertex0],asEdgeNorm[vertex1],asEdgeNorm[vertex2]);
    	}
    };

    /**********         Data Access Methods        ************/
    template <typename DATA_TYPE> inline DATA_TYPE getDiscreetValue (const DATA_TYPE* data, const int &x, const int &y, const int &z)
    {
	    //ADDED BY ME (the if else structure)
	    if (octree == NULL)
	    {
		    return data[(x + width)%width + ((y + height)%height)*width + ((z + depth)%depth)*width*height];	    
	    }
	    else
	    {		    
		    if ((x >= octree->getSize()) || (y >= octree->getSize()) || (z >= octree->getSize()) || (x < 0) || (y < 0) || (z < 0))
		    {
			    return SPACE_EMPTY;
		    }
		    else
		    {
			    return octree->at(x,y,z).solid;
		    }		    
	    }    	
    };



    /**********         HELPER FUNCTIONS         ************/

    /*
    getOffset finds the approximate point of intersection of the surface
    between two points with the values fValue1 and fValue2
    */
    template <typename DATA_TYPE,typename TARGET_TYPE> float getOffset(const DATA_TYPE &fValue1, const DATA_TYPE &fValue2, const TARGET_TYPE &fValueDesired)
    {
    	DATA_TYPE fDelta = fValue2 - fValue1;
    	if(fDelta == 0.0) { return 0.5; }
    	return (fValueDesired - fValue1)/fDelta;
    };







    //getNormal() finds the gradient of the scalar field at a point
    //This gradient can be used as a very accurate vertx normal for lighting calculations
    template <typename DATA_TYPE> void getNormal(Vec3f &rfNormal,  const int &x, const int &y, const int &z, const DATA_TYPE* data)
    {
    	rfNormal.x =  getDiscreetValue<DATA_TYPE>(data, x-1, y, z) - getDiscreetValue<DATA_TYPE>(data, x+1, y, z);
    	rfNormal.y =  getDiscreetValue<DATA_TYPE>(data, x, y-1, z) - getDiscreetValue<DATA_TYPE>(data, x, y+1, z);
    	rfNormal.z =  getDiscreetValue<DATA_TYPE>(data, x, y, z-1) - getDiscreetValue<DATA_TYPE>(data, x, y, z+1);
    	normalizeVector(rfNormal, rfNormal);
    };

    void normalizeVector(Vec3f &rfVectorResult, Vec3f &rfVectorSource);

    //a2fVertexOffset lists the positions, relative to vertex0, of each of the 8 vertices of a cube
    static const float a2fVertexOffset[8][3];
    //a2iEdgeConnection lists the index of the endpoint vertices for each of the 12 edges of the cube
    static const int a2iEdgeConnection[12][2];

    //a2fEdgeDirection lists the direction vector (vertex1-vertex0) for each edge in the cube
    static const float a2fEdgeDirection[12][3];

    //a2iTetrahedronEdgeConnection lists the index of the endpoint vertices for each of the 6 edges of the tetrahedron
    static const int a2iTetrahedronEdgeConnection[6][2];

    //a2iTetrahedronEdgeConnection lists the index of verticies from a cube
    // that made up each of the six tetrahedrons within the cube
    static const int a2iTetrahedronsInACube[6][4];

    static const int aiCubeEdgeFlags[256];

    static const int aiTetrahedronEdgeFlags[16];

    static const int a2iTetrahedronTriangles[16][7];


    static const int a2iTriangleConnectionTable[256][16];
};

#endif /* GEOMETRYEXTRACTOR_H_ */
