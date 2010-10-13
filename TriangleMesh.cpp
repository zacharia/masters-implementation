#include "TriangleMesh.h"

#include <fstream>
using namespace std;

ofstream file("rot.txt");

TriangleMesh::TriangleMesh()
{
	vertices = std::vector<Vec3f>();
	triangles = std::vector<Triangle>();
	normals = std::vector<Vec3f>();
	clearGeometry();
}

TriangleMesh::TriangleMesh(const TriangleMesh& meshIn) : vertices(meshIn.vertices), normals(meshIn.normals), triangles(meshIn.triangles)
{
}

bool TriangleMesh::isEmpty()
{
	return (triangles.size() == 0);
}

TriangleMesh::~TriangleMesh()
{
	clearGeometry();
}

void TriangleMesh::clearGeometry()
{
	vertices.clear();
	normals.clear();
	triangles.clear();
}

Vec3f& TriangleMesh::getVert(int tri, int index)
{
	switch (index)
	{
	case 0:
		return vertices[triangles[tri].a];
	case 1:
		return vertices[triangles[tri].b];
	case 2:
		return vertices[triangles[tri].c];
	}

	return vertices[triangles[0].a];
}

Vec3f& TriangleMesh::getVert(std::vector<Triangle> trianglesIn, int tri, int index)
{
	switch (index)
	{
	case 0:
		return vertices[trianglesIn[tri].a];
	case 1:
		return vertices[trianglesIn[tri].b];
	case 2:
		return vertices[trianglesIn[tri].c];
	}

	return vertices[trianglesIn[0].a];
}

Vec3f& TriangleMesh::getVert(int index)
{
	return vertices[index];
}

Vec3f& TriangleMesh::getNorm(std::vector<Triangle> trianglesIn, int tri, int index)
{
	if (normals.size() != 0)
		switch (index)
		{
		case 0:
			return normals[trianglesIn[tri].na];
		case 1:
			return normals[trianglesIn[tri].nb];
		case 2:
			return normals[trianglesIn[tri].nc];
		}

	return normals[trianglesIn[0].na];
}

Vec3f& TriangleMesh::getNorm(int tri, int index)
{
	if (normals.size() != 0)
		switch (index)
		{
		case 0:
			return normals[triangles[tri].na];
		case 1:
			return normals[triangles[tri].nb];
		case 2:
			return normals[triangles[tri].nc];
		}

	return normals[triangles[tri].na];
}

void TriangleMesh::addTriangle(Vec3f verta, Vec3f vertb, Vec3f vertc,
		Vec3f norma, Vec3f normb, Vec3f normc)
{
	Triangle curr;
	int startVertIndex = vertices.size(), startNormIndex = normals.size();

	vertices.push_back(verta);
	vertices.push_back(vertb);
	vertices.push_back(vertc);

	normals.push_back(norma);
	normals.push_back(normb);
	normals.push_back(normc);

	curr.a = startVertIndex;
	curr.b = startVertIndex + 1;
	curr.c = startVertIndex + 2;

	curr.na = startNormIndex;
	curr.nb = startNormIndex + 1;
	curr.nc = startNormIndex + 2;

	triangles.push_back(curr);

}

void TriangleMesh::addTriangle(int a, int b, int c, int na, int nb, int nc)
{
	Triangle curr;

	curr.a = a;
	curr.b = b;
	curr.c = c;

	curr.na = na;
	curr.nb = nb;
	curr.nc = nc;

	triangles.push_back(curr);

}

void TriangleMesh::centroidMesh ()
{
	Vec3f average (0,0,0);
	for (std::vector<Vec3f>::iterator i = vertices.begin(); i != vertices.end() ; ++i)
	{
		average += *i;
	}
	average/=vertices.size();
	for (std::vector<Vec3f>::iterator i = vertices.begin(); i != vertices.end() ; ++i)
	{
		*i -= average;
	}
}

void TriangleMesh::centreMesh (int width, int height, int depth)
{
	Vec3f offset (width/2,height/2,depth/2);
	for (std::vector<Vec3f>::iterator i = vertices.begin(); i != vertices.end() ; ++i)
	{
		*i-=offset;
	}
}
