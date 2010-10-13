#include "MeshExtractor.h"
/*
 * Adapted From
 * marching_cubes.cpp
 *  MC Algorithm implemented by Paul Bourke wiki contribution
 *  Qt-Adaption Created on: 15.07.2009
 *      Author: manitoo
 *  Available: http://local.wasp.uwa.edu.au/~pbourke/geometry/polygonise/index.html
 */




//add discreet function here
float MeshExtractor::getDiscreetValueCartesianGrid( const int &x, const int &y, const int &z)
{
	return data[(x + width)%width + ((y + height)%height)*width + ((z + depth)%depth)*width*height];
}

float MeshExtractor::getDiscreetValueInvertedGrid( const int &x, const int &y, const int &z)
{
	return data[(width - x -1 + width)%width + ((height - y -1 + height)%height)*width + ((depth - z - 1 + depth)%depth)*width*height];
}

void MeshExtractor::setData (float *dataIn, int widthIn, int heightIn, int depthIn)
{
	data = dataIn;
	width = widthIn;
	height = heightIn;
	depth = depthIn;
}

MeshExtractor::MeshExtractor()
{
    //Initialize Control Variables
    LEVEL_SET_TYPE      = 1;
    PARTITION_MODE      = 1;
    POLYGON_MODE        = 1;
    PROJECTION_MODE     = 1;
    BLENDING_MODE       = 1;


    //this is need for a continuous range
    //stepSize       = ?;

}

MeshExtractor::~MeshExtractor()
{
}

void MeshExtractor::extractMesh(TriangleMesh* mesh, float* dataIn, bool dataInverted, int widthIn, int heightIn, int depthIn, int partition, float targetValue)
{
	setData (dataIn, widthIn, heightIn, depthIn);
    //Select Partition Mode
    switch ( partition )
    {
        case 1:
        	marchCube = &MeshExtractor::marchCube1;
        	break;
        case 2:
        	marchCube = &MeshExtractor::marchCube2;
        	break;
        default:		
        	break;
    }

    if (dataInverted)
    	getDiscreetValue = &MeshExtractor::getDiscreetValueInvertedGrid;
    else
    	getDiscreetValue = &MeshExtractor::getDiscreetValueCartesianGrid;

    //Start Computation
    for(int x = 0; x< width; ++x)
    	for(int y = 0; y < height; ++y)
			for(int z = 0; z < depth; ++z)
				(this->*marchCube)(mesh, x, y, z, targetValue);

				//this is need for a continuous range
				//(this->*marchCube)(x*stepSize, y*stepSize, z*stepSize, targetValue);

    //mesh->centreMesh(width, height, depth);
}


/*
getOffset finds the approximate point of intersection of the surface
between two points with the values fValue1 and fValue2
*/
float MeshExtractor::getOffset(const float &fValue1, const float &fValue2, const float &fValueDesired)
{
	float fDelta = fValue2 - fValue1;
	if(fDelta == 0.0) { return 0.5; }
	return (fValueDesired - fValue1)/fDelta;
}

//vGetColor generates a color from a given position and normal of a point
void MeshExtractor::getColor(Vec3f &rfColor, Vec3f &rfPosition, Vec3f &rfNormal)
{
	double x = rfNormal.x;
	double y = rfNormal.y;
	double z = rfNormal.z;
	rfColor.x = ((x > 0.0 ? x : 0.0) + (y < 0.0 ? -0.5*y : 0.0) + (z < 0.0 ? -0.5*z : 0.0));
	rfColor.y = ((y > 0.0 ? y : 0.0) + (z < 0.0 ? -0.5*z : 0.0) + (x < 0.0 ? -0.5*x : 0.0));
	rfColor.z = ((z > 0.0 ? z : 0.0) + (x < 0.0 ? -0.5*x : 0.0) + (y < 0.0 ? -0.5*y : 0.0));
}

void MeshExtractor::normalizeVector(Vec3f &rfVectorResult, Vec3f &rfVectorSource)
{
	float fOldLength;
	float fScale;

	fOldLength = sqrt( (rfVectorSource.x * rfVectorSource.x) +
						(rfVectorSource.y * rfVectorSource.y) +
						(rfVectorSource.z * rfVectorSource.z) );

	if(fOldLength == 0.0)
	{
			rfVectorResult.x = ( rfVectorSource.x);
			rfVectorResult.y = ( rfVectorSource.y);
			rfVectorResult.z = ( rfVectorSource.z);
	}
	else
	{
			fScale = 1.0/fOldLength;
			rfVectorResult.x = ( rfVectorSource.x*fScale);
			rfVectorResult.y = ( rfVectorSource.y*fScale);
			rfVectorResult.z = ( rfVectorSource.z*fScale);
	}
}



//vGetNormal() finds the gradient of the scalar field at a point
//This gradient can be used as a very accurate vertx normal for lighting calculations
void MeshExtractor::getNormal(Vec3f &rfNormal, const int &x, const int &y, const int &z)
{
	rfNormal.x = ( (this->*getDiscreetValue)(x-1, y, z) - (this->*getDiscreetValue)(x+1, y, z));
	rfNormal.y = ( (this->*getDiscreetValue)(x, y-1, z) - (this->*getDiscreetValue)(x, y+1, z));
	rfNormal.z = ( (this->*getDiscreetValue)(x, y, z-1) - (this->*getDiscreetValue)(x, y, z+1));
	normalizeVector(rfNormal, rfNormal);
}

//marchCube1 performs the Marching Cubes algorithm on a single cube
void MeshExtractor::marchCube1(TriangleMesh* meshOut, const int &x, const int &y, const int &z, const float &targetValue)
{
	int iVertex, vertex0, vertex1, vertex2, iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
	float fOffset;
	Vec3f sColor;
	float afCubeValue[8];
	Vec3f asEdgeVertex[12];
	Vec3f asEdgeNorm[12];

	//Make a local copy of the values at the cube's corners
	for(iVertex = 0; iVertex < 8; ++iVertex)
	{
		afCubeValue[iVertex] = (this->*getDiscreetValue)(x + a2fVertexOffset[iVertex][0],y + a2fVertexOffset[iVertex][1],z + a2fVertexOffset[iVertex][2]);
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
			return;
	}

	//Find the point of intersection of the surface with each edge
	//Then find the normal to the surface at those points
	for(iEdge = 0; iEdge < 12; ++iEdge)
	{
		//if there is an intersection on this edge
		if(iEdgeFlags & (1<<iEdge))
		{
			fOffset = getOffset(afCubeValue[ a2iEdgeConnection[iEdge][0] ],afCubeValue[ a2iEdgeConnection[iEdge][1] ], targetValue);

			asEdgeVertex[iEdge].x = ( x + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][0]  +  fOffset * a2fEdgeDirection[iEdge][0]));
			asEdgeVertex[iEdge].y = ( y + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][1]  +  fOffset * a2fEdgeDirection[iEdge][1]));
			asEdgeVertex[iEdge].z = ( z + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][2]  +  fOffset * a2fEdgeDirection[iEdge][2]));

			getNormal(asEdgeNorm[iEdge], asEdgeVertex[iEdge].x, asEdgeVertex[iEdge].y, asEdgeVertex[iEdge].z);
		}
	}


        //Draw the triangles that were found.  There can be up to five per cube
	for(iTriangle = 0; iTriangle < 5; ++iTriangle)
	{
		if(a2iTriangleConnectionTable[iFlagIndex][3*iTriangle] < 0) break;

		vertex0 = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle];
		vertex1 = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+1];
		vertex2 = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+2];

		meshOut->addTriangle(asEdgeVertex[vertex0],asEdgeVertex[vertex1],asEdgeVertex[vertex2],asEdgeNorm[vertex0],asEdgeNorm[vertex1],asEdgeNorm[vertex2]);
	}
}

//marchCube2 performs the Marching Tetrahedrons algorithm on a single cube by making six calls to marchTetrahedron
void MeshExtractor::marchCube2(TriangleMesh* meshOut, const int &x, const int &y, const int &z, const float &targetValue)
{
	int iVertex, iTetrahedron, iVertexInACube;
	Vec3f asCubePosition[8];
	float  afCubeValue[8];
	Vec3f asTetrahedronPosition[4];
	float  afTetrahedronValue[4];

	//Make a local copy of the cube's corner positions
	for(iVertex = 0; iVertex < 8; ++iVertex)
	{
			asCubePosition[iVertex].x = ( x + a2fVertexOffset[iVertex][0]);
			asCubePosition[iVertex].y = ( y + a2fVertexOffset[iVertex][1]);
			asCubePosition[iVertex].z = ( z + a2fVertexOffset[iVertex][2]);
	}

	//Make a local copy of the cube's corner values
	for(iVertex = 0; iVertex < 8; ++iVertex)
	{
		afCubeValue[iVertex] = (this->*getDiscreetValue)(asCubePosition[iVertex].x,asCubePosition[iVertex].y,asCubePosition[iVertex].z);
	}

	for(iTetrahedron = 0; iTetrahedron < 6; ++iTetrahedron)
	{
		for(iVertex = 0; iVertex < 4; ++iVertex)
		{
			iVertexInACube = a2iTetrahedronsInACube[iTetrahedron][iVertex];
			asTetrahedronPosition[iVertex].x = ( asCubePosition[iVertexInACube].x);
			asTetrahedronPosition[iVertex].y = ( asCubePosition[iVertexInACube].y);
			asTetrahedronPosition[iVertex].z = ( asCubePosition[iVertexInACube].z);
			afTetrahedronValue[iVertex] = afCubeValue[iVertexInACube];
		}
		marchTetrahedron(meshOut, asTetrahedronPosition, afTetrahedronValue, targetValue);
	}
}


//marchTetrahedron performs the Marching Tetrahedrons algorithm on a single tetrahedron
void MeshExtractor::marchTetrahedron(TriangleMesh* meshOut, Vec3f *pasTetrahedronPosition, float *pafTetrahedronValue, float targetValue)
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
			fOffset = getOffset(pafTetrahedronValue[iVert0], pafTetrahedronValue[iVert1], targetValue);
			fInvOffset = 1.0 - fOffset;

			asEdgeVertex[iEdge].x = ( fInvOffset*pasTetrahedronPosition[iVert0].x  +  fOffset*pasTetrahedronPosition[iVert1].x);
			asEdgeVertex[iEdge].y = ( fInvOffset*pasTetrahedronPosition[iVert0].y  +  fOffset*pasTetrahedronPosition[iVert1].y);
			asEdgeVertex[iEdge].z = ( fInvOffset*pasTetrahedronPosition[iVert0].z  +  fOffset*pasTetrahedronPosition[iVert1].z);

			getNormal(asEdgeNorm[iEdge], asEdgeVertex[iEdge].x, asEdgeVertex[iEdge].y, asEdgeVertex[iEdge].z);
		}
	}
	//Draw the triangles that were found.  There can be up to 2 per tetrahedron
	for(iTriangle = 0; iTriangle < 2; ++iTriangle)
	{
		if(a2iTetrahedronTriangles[iFlagIndex][3*iTriangle] < 0)    break;

		vertex0 = a2iTetrahedronTriangles[iFlagIndex][3*iTriangle];
		vertex1 = a2iTetrahedronTriangles[iFlagIndex][3*iTriangle+1];
		vertex2 = a2iTetrahedronTriangles[iFlagIndex][3*iTriangle+2];

		meshOut->addTriangle(asEdgeVertex[vertex0],asEdgeVertex[vertex1],asEdgeVertex[vertex2],asEdgeNorm[vertex0],asEdgeNorm[vertex1],asEdgeNorm[vertex2]);
	}
}
