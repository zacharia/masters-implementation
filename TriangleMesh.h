#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H

#include <string>
#include <iostream>
#include <vector>
/* #include <QVector2D> */
/* #include <QVector3D> */
#include <OGRE/Ogre.h>

/* typedef QVector2D Vec2f; */
/* typedef QVector3D Vec3f; */

typedef Ogre::Vector2 Vec2f;
typedef Ogre::Vector3 Vec3f;


class Triangle
{
public:
	int a, b, c; 	 //vertex indices
	int na, nb, nc;  //normal indices

	Triangle () : a(0), b(0), c(0), na(0), nb(0), nc(0)
	{
	}

	Triangle (const Triangle& triangle) : a(triangle.a), b(triangle.b), c(triangle.c), na(triangle.na), nb(triangle.nb), nc(triangle.nc)
	{
	}
};

class TriangleMesh
{

public:

	std::vector<Triangle> triangles;
	std::vector<Vec3f> vertices;
	std::vector<Vec3f> normals;


	TriangleMesh();
	TriangleMesh(const TriangleMesh& meshIn);

	~TriangleMesh();

	bool isEmpty();

	void clearGeometry();
	Vec3f& getVert(int tri, int index);
	Vec3f& getVert(std::vector<Triangle> trianglesIn, int tri, int index);
	Vec3f& getVert(int index);
	Vec3f& getNorm(std::vector<Triangle> trianglesIn, int tri, int index);
	Vec3f& getNorm(int tri, int index);
	void addTriangle(Vec3f verta, Vec3f vertb, Vec3f vertc, Vec3f norma,
			Vec3f normb, Vec3f normc);
	void addTriangle(int a, int b, int c, int na, int nb, int nc);
	void centreMesh (const int &width, const int& height, const int& depth);
	void centroidMesh ();
};

#endif // TRIANGLE_MESH_H
