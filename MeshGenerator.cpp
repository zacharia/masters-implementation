/*
 */

#include "MeshGenerator.h"

MeshGenerator::MeshGenerator()
{
	
}


MeshGenerator::~MeshGenerator()
{
	
}


//fGetOffset finds the approximate point of intersection of the surface
// between two points with the values fValue1 and fValue2
float MeshGenerator::fGetOffset(float fValue1, float fValue2, float fValueDesired)
{
        double fDelta = fValue2 - fValue1;

        if(fDelta == 0.0)
        {
                return 0.5;
        }
        return (fValueDesired - fValue1)/fDelta;
}


//vGetColor generates a color from a given position and normal of a point
Ogre::Vector3 MeshGenerator::vGetColor(Ogre::Vector3 &rfPosition, Ogre::Vector3 &rfNormal)
{
	Ogre::Vector3 rfColor;
	
        float fX = rfNormal.x;
        float fY = rfNormal.y;
        float fZ = rfNormal.z;
        rfColor.x = (fX > 0.0 ? fX : 0.0) + (fY < 0.0 ? -0.5*fY : 0.0) + (fZ < 0.0 ? -0.5*fZ : 0.0);
        rfColor.y = (fY > 0.0 ? fY : 0.0) + (fZ < 0.0 ? -0.5*fZ : 0.0) + (fX < 0.0 ? -0.5*fX : 0.0);
        rfColor.z = (fZ > 0.0 ? fZ : 0.0) + (fX < 0.0 ? -0.5*fX : 0.0) + (fY < 0.0 ? -0.5*fY : 0.0);

	return rfColor;
}


Ogre::Vector3 MeshGenerator::vNormalizeVector(Ogre::Vector3 &rfVectorSource)
{
        float fOldLength;
        float fScale;

	Ogre::Vector3 rfVectorResult;

        fOldLength = sqrtf( (rfVectorSource.x * rfVectorSource.x) +
                            (rfVectorSource.y * rfVectorSource.y) +
                            (rfVectorSource.z * rfVectorSource.z) );

        if(fOldLength == 0.0)
        {
                rfVectorResult.x = rfVectorSource.x;
                rfVectorResult.y = rfVectorSource.y;
                rfVectorResult.z = rfVectorSource.z;
        }
        else
        {
                fScale = 1.0/fOldLength;
                rfVectorResult.x = rfVectorSource.x*fScale;
                rfVectorResult.y = rfVectorSource.y*fScale;
                rfVectorResult.z = rfVectorSource.z*fScale;
        }

	return rfVectorResult;
}


float MeshGenerator::fSample(float fX, float fY, float fZ)
{
	return 0;
}


//vGetNormal() finds the gradient of the scalar field at a point
//This gradient can be used as a very accurate vertx normal for lighting calculations
Ogre::Vector3 MeshGenerator::vGetNormal(float fX, float fY, float fZ)
{
	Ogre::Vector3 rfNormal;
	
        rfNormal.x = fSample(fX-0.01, fY, fZ) - fSample(fX+0.01, fY, fZ);
        rfNormal.y = fSample(fX, fY-0.01, fZ) - fSample(fX, fY+0.01, fZ);
        rfNormal.z = fSample(fX, fY, fZ-0.01) - fSample(fX, fY, fZ+0.01);
        vNormalizeVector(rfNormal);

	return rfNormal;
}


//vMarchCube1 performs the Marching Cubes algorithm on a single cube
void MeshGenerator::vMarchCube1(float fX, float fY, float fZ, float fScale)
{
        extern int aiCubeEdgeFlags[256];
        extern int a2iTriangleConnectionTable[256][16];

        int iCorner, iVertex, iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
        float fOffset;
        Ogre::Vector3 sColor;
        float afCubeValue[8];
        Ogre::Vector3 asEdgeVertex[12];
        Ogre::Vector3 asEdgeNorm[12];

        //Make a local copy of the values at the cube's corners
        for(iVertex = 0; iVertex < 8; iVertex++)
        {
                afCubeValue[iVertex] = fSample(fX + a2fVertexOffset[iVertex][0]*fScale,
					       fY + a2fVertexOffset[iVertex][1]*fScale,
					       fZ + a2fVertexOffset[iVertex][2]*fScale);
        }

        //Find which vertices are inside of the surface and which are outside
        iFlagIndex = 0;
        for(iVertexTest = 0; iVertexTest < 8; iVertexTest++)
        {
                if(afCubeValue[iVertexTest] <= fTargetValue) 
                        iFlagIndex |= 1<<iVertexTest;
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
        for(iEdge = 0; iEdge < 12; iEdge++)
        {
                //if there is an intersection on this edge
                if(iEdgeFlags & (1<<iEdge))
                {
                        fOffset = fGetOffset(afCubeValue[ a2iEdgeConnection[iEdge][0] ], 
					     afCubeValue[ a2iEdgeConnection[iEdge][1] ], fTargetValue);

                        asEdgeVertex[iEdge].x = fX + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][0]  +  fOffset * a2fEdgeDirection[iEdge][0]) * fScale;
                        asEdgeVertex[iEdge].y = fY + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][1]  +  fOffset * a2fEdgeDirection[iEdge][1]) * fScale;
                        asEdgeVertex[iEdge].z = fZ + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][2]  +  fOffset * a2fEdgeDirection[iEdge][2]) * fScale;

                        asEdgeNorm[iEdge] = vGetNormal(asEdgeVertex[iEdge].x, asEdgeVertex[iEdge].y, asEdgeVertex[iEdge].z);
                }
        }


        //Draw the triangles that were found.  There can be up to five per cube
        for(iTriangle = 0; iTriangle < 5; iTriangle++)
        {
                if(a2iTriangleConnectionTable[iFlagIndex][3*iTriangle] < 0)
                        break;

                for(iCorner = 0; iCorner < 3; iCorner++)
                {
                        iVertex = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+iCorner];

                        sColor = vGetColor(asEdgeVertex[iVertex], asEdgeNorm[iVertex]);
                        // glColor3f(sColor.x, sColor.y, sColor.z);
                        // glNormal3f(asEdgeNorm[iVertex].x,   asEdgeNorm[iVertex].y,   asEdgeNorm[iVertex].z);
                        // glVertex3f(asEdgeVertex[iVertex].x, asEdgeVertex[iVertex].y, asEdgeVertex[iVertex].z);
                }
        }
}


//vMarchTetrahedron performs the Marching Tetrahedrons algorithm on a single tetrahedron
void MeshGenerator::vMarchTetrahedron(Ogre::Vector3 *pasTetrahedronPosition, float *pafTetrahedronValue)
{
        extern int aiTetrahedronEdgeFlags[16];
        extern int a2iTetrahedronTriangles[16][7];

        int iEdge, iVert0, iVert1, iEdgeFlags, iTriangle, iCorner, iVertex, iFlagIndex = 0;
        float fOffset, fInvOffset, fValue = 0.0;
        Ogre::Vector3 asEdgeVertex[6];
        Ogre::Vector3 asEdgeNorm[6];
        Ogre::Vector3 sColor;

        //Find which vertices are inside of the surface and which are outside
        for(iVertex = 0; iVertex < 4; iVertex++)
        {
                if(pafTetrahedronValue[iVertex] <= fTargetValue) 
                        iFlagIndex |= 1<<iVertex;
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
        for(iEdge = 0; iEdge < 6; iEdge++)
        {
                //if there is an intersection on this edge
                if(iEdgeFlags & (1<<iEdge))
                {
                        iVert0 = a2iTetrahedronEdgeConnection[iEdge][0];
                        iVert1 = a2iTetrahedronEdgeConnection[iEdge][1];
                        fOffset = fGetOffset(pafTetrahedronValue[iVert0], pafTetrahedronValue[iVert1], fTargetValue);
                        fInvOffset = 1.0 - fOffset;

                        asEdgeVertex[iEdge].x = fInvOffset*pasTetrahedronPosition[iVert0].x  +  fOffset*pasTetrahedronPosition[iVert1].x;
                        asEdgeVertex[iEdge].y = fInvOffset*pasTetrahedronPosition[iVert0].y  +  fOffset*pasTetrahedronPosition[iVert1].y;
                        asEdgeVertex[iEdge].z = fInvOffset*pasTetrahedronPosition[iVert0].z  +  fOffset*pasTetrahedronPosition[iVert1].z;
                        
                        asEdgeNorm[iEdge] = vGetNormal(asEdgeVertex[iEdge].x, asEdgeVertex[iEdge].y, asEdgeVertex[iEdge].z);
                }
        }
        //Draw the triangles that were found.  There can be up to 2 per tetrahedron
        for(iTriangle = 0; iTriangle < 2; iTriangle++)
        {
                if(a2iTetrahedronTriangles[iFlagIndex][3*iTriangle] < 0)
                        break;

                for(iCorner = 0; iCorner < 3; iCorner++)
                {
                        iVertex = a2iTetrahedronTriangles[iFlagIndex][3*iTriangle+iCorner];

                        sColor = vGetColor(asEdgeVertex[iVertex], asEdgeNorm[iVertex]);
                        // glColor3f(sColor.x, sColor.y, sColor.z);
                        // glNormal3f(asEdgeNorm[iVertex].x,   asEdgeNorm[iVertex].y,   asEdgeNorm[iVertex].z);
                        // glVertex3f(asEdgeVertex[iVertex].x, asEdgeVertex[iVertex].y, asEdgeVertex[iVertex].z);
                }
        }
}


//vMarchCube2 performs the Marching Tetrahedrons algorithm on a single cube by making six calls to vMarchTetrahedron
void MeshGenerator::vMarchCube2(float fX, float fY, float fZ, float fScale)
{
        int iVertex, iTetrahedron, iVertexInACube;
        Ogre::Vector3 asCubePosition[8];
        float  afCubeValue[8];
        Ogre::Vector3 asTetrahedronPosition[4];
        float  afTetrahedronValue[4];

        //Make a local copy of the cube's corner positions
        for(iVertex = 0; iVertex < 8; iVertex++)
        {
                asCubePosition[iVertex].x = fX + a2fVertexOffset[iVertex][0]*fScale;
                asCubePosition[iVertex].y = fY + a2fVertexOffset[iVertex][1]*fScale;
                asCubePosition[iVertex].z = fZ + a2fVertexOffset[iVertex][2]*fScale;
        }

        //Make a local copy of the cube's corner values
        for(iVertex = 0; iVertex < 8; iVertex++)
        {
                afCubeValue[iVertex] = fSample(asCubePosition[iVertex].x,
					       asCubePosition[iVertex].y,
                                               asCubePosition[iVertex].z);
        }

        for(iTetrahedron = 0; iTetrahedron < 6; iTetrahedron++)
        {
                for(iVertex = 0; iVertex < 4; iVertex++)
                {
                        iVertexInACube = a2iTetrahedronsInACube[iTetrahedron][iVertex];
                        asTetrahedronPosition[iVertex].x = asCubePosition[iVertexInACube].x;
                        asTetrahedronPosition[iVertex].y = asCubePosition[iVertexInACube].y;
                        asTetrahedronPosition[iVertex].z = asCubePosition[iVertexInACube].z;
                        afTetrahedronValue[iVertex] = afCubeValue[iVertexInACube];
                }
                vMarchTetrahedron(asTetrahedronPosition, afTetrahedronValue);
        }
}
        

//vMarchingCubes iterates over the entire dataset, calling vMarchCube on each cube
void MeshGenerator::vMarchingCubes()
{
        int iX, iY, iZ;
        for(iX = 0; iX < iDataSetSize; iX++)
		for(iY = 0; iY < iDataSetSize; iY++)
			for(iZ = 0; iZ < iDataSetSize; iZ++)
			{
				vMarchCube1(iX*fStepSize, iY*fStepSize, iZ*fStepSize, fStepSize);
			}
}


//vMarchingCubes iterates over the entire dataset, calling vMarchCube on each cube
void MeshGenerator::vMarchingTetrahedrons()
{
        int iX, iY, iZ;
        for(iX = 0; iX < iDataSetSize; iX++)
		for(iY = 0; iY < iDataSetSize; iY++)
			for(iZ = 0; iZ < iDataSetSize; iZ++)
			{
				vMarchCube2(iX*fStepSize, iY*fStepSize, iZ*fStepSize, fStepSize);
			}
}
