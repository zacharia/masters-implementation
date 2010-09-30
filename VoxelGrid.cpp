/*

*/

#include "VoxelGrid.h"

VoxelGrid::VoxelGrid()
{
	grid = NULL;
}

VoxelGrid::VoxelGrid(int size)
{
	grid = NULL;
	makeVoxelGrid(size);
}

VoxelGrid::~VoxelGrid()
{
	if (grid != NULL)
	{
		delete grid;
	}
}

//this method initializes the actual 3d array that represents the grid.
void VoxelGrid::makeVoxelGrid(int size)
{
	//TODO: parametize this correctly
	grid = new Octree<unsigned int, 32>(size, 0);
}

//this method displays the voxel grid on the command line
void VoxelGrid::displayVoxelGrid()
{
	std::cout << "\n\n";
	//TODO: this needs to be redone, if it's even needed at all
}

int VoxelGrid::getSize()
{
	return size;
}

//returns the value at a point in the grid
unsigned int VoxelGrid::getValue(int x, int y, int z)
{
	//if the requested position is invalid, return 0xff
	if ((x < 0) || (y < 0) || (z < 0) || (x > size) || (y > size) || (z > size))
	{
		return 0;
	}
	else
	{
		return grid->at(x,y,z);
	}
}

// void VoxelGrid::makeCircle()
// {
// 	for (int i = 0; i < size; i++)
// 	{
// 	       	for (int j = 0; j < size; j++)
// 		{
// 			for (int k = 0; k < size; k++)
// 			{
// 				if ((abs(i - j) < 5) || (abs(j - k) < 5) || (abs(i - k) < 5))
// 				{
// 					grid[i][j][k] = 0x01;
// 				}
// 			}
// 		}
// 	}
// }

//pos is the centre of the rectangle, extents are the dimensions from the centre to the edges
void VoxelGrid::makeRectangle(Ogre::Vector3 pos, Ogre::Vector3 extents, Ogre::Matrix3 orientation, bool add)
{
	for (int i = -extents.x; i <= extents.x; i++)
	{
		for (int j = -extents.y; j <= extents.y; j++)
		{
			for (int k = -extents.z; k <= extents.z; k++)
			{
				//check the current voxel is in bounds
				if ((pos.x + i < size) &&
				    (pos.x + i >= 0) &&
				    (pos.y + j < size) &&
				    (pos.y + j >= 0) &&
				    (pos.z + k < size) &&
				    (pos.z + k >= 0))
				{
					if (add)
					{
						//size(int)pos.x + i][(int)pos.y + j][(int)pos.z + k] = 0x01;
					}
					else
					{
						//size(int)pos.x + i][(int)pos.y + j][(int)pos.z + k] = 0x00;
					}
				}
			}
		}
	}
}

//pos is the centre of the ellipsoid, extents are the dimensions from the centre to the edges
void VoxelGrid::makeEllipsoid(Ogre::Vector3 pos, Ogre::Vector3 extents, Ogre::Matrix3 orientation, bool add)
{
	for (int i = -extents.x; i <= extents.x; i++)
	{
		for (int j = -extents.y; j <= extents.y; j++)
		{
			for (int k = -extents.z; k <= extents.z; k++)
			{
				//check the current voxel is in bounds
				if ((pos.x + i < size) &&
				    (pos.x + i >= 0) &&
				    (pos.y + j < size) &&
				    (pos.y + j >= 0) &&
				    (pos.z + k < size) &&
				    (pos.z + k >= 0))
				{
					if (pow((i) / (float)extents.x, 2) + pow((j) / (double)extents.y, 2) + pow((k) / (double)extents.z, 2) <= 1)
					{
						if (add)
						{
							//size(int)pos.x + i][(int)pos.y + j][(int)pos.z + k] = 0x01;
						}
						else
						{
							//size(int)pos.x + i][(int)pos.y + j][(int)pos.z + k] = 0x00;
						}
					}
				}
			}
		}
	}
}

//pos is the centre of the ellipsoid, extents are the dimensions from the centre to the edges.
//orientation is either x, y or z and indicates which axis the cylinder runs parallel to
void VoxelGrid::makeCylinder(Ogre::Vector3 pos, Ogre::Vector3 extents, Ogre::Matrix3 orientation, bool add)
{
	for (int i = -extents.x; i <= extents.x; i++)
	{
		for (int j = -extents.y; j <= extents.y; j++)
		{
			for (int k = -extents.z; k <= extents.z; k++)
			{
				//check the current voxel is in bounds
				if ((pos.x + i < size) &&
				    (pos.x + i >= 0) &&
				    (pos.y + j < size) &&
				    (pos.y + j >= 0) &&
				    (pos.z + k < size) &&
				    (pos.z + k >= 0))
				{
					// if ((orientation == 'x') && (pow((j) / (double)extents.y, 2) + pow((k) / (double)extents.z, 2) <= 1))
					// {
					// 	if (add)
					// 	{
					// 		//size(int)pos.x + i][(int)pos.y + j][(int)pos.z + k] = 0x01;
					// 	}
					// 	else
					// 	{
					// 		//size(int)pos.x + i][(int)pos.y + j][(int)pos.z + k] = 0x00;
					// 	}
					// }
					// else if ((orientation == 'y') && (pow((i) / (double)extents.x, 2) + pow((k) / (double)extents.z, 2) <= 1))
					// {
					// 	if (add)
					// 	{
					// 		//size(int)pos.x + i][(int)pos.y + j][(int)pos.z + k] = 0x01;
					// 	}
					// 	else
					// 	{
					// 		//grid[(int)pos.x + i][(int)pos.y + j][(int)pos.z + k] = 0x00;
					// 	}
					// }
					// else if ((orientation == 'z') && (pow((j) / (double)extents.y, 2) + pow((i) / (double)extents.x, 2) <= 1))
					// {
					// 	if (add)
					// 	{
					// 		//grid[(int)pos.x + i][(int)pos.y + j][(int)pos.z + k] = 0x01;
					// 	}
					// 	else
					// 	{
					// 		//grid[(int)pos.x + i][(int)pos.y + j][(int)pos.z + k] = 0x00;
					// 	}
					// }
				}
			}
		}
	}
}
