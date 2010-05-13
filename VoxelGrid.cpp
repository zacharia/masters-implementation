/*

*/

#include "VoxelGrid.h"

VoxelGrid::VoxelGrid()
{
	grid = NULL;
}

VoxelGrid::VoxelGrid(int x, int y, int z)
{
	grid = NULL;
	makeVoxelGrid(x, y, z);
}

VoxelGrid::~VoxelGrid()
{
	if (grid != NULL)
	{
		for (int i = 0; i < gridX; i++)
		{
			for (int j = 0; j < gridY; j++)
			{
				delete [] grid[i][j];
			}
			delete [] grid[i];
		}
		delete [] grid;
	}
}

//this method initializes the actual 3d array that represents the grid.
void VoxelGrid::makeVoxelGrid(int x, int y, int z)
{
	//assign the correct values to the size variables
	gridX = x;
	gridY = y;
	gridZ = z;

	//make the voxel array
	grid = new char**[gridX];
	for (int i = 0; i < gridX; i++)
	{
		grid[i] = new char*[gridY];
		for (int j = 0; j < gridY; j++)
		{
			grid[i][j] = new char[gridZ];
		}
	}
}

//this method displays the voxel grid on the command line
void VoxelGrid::displayVoxelGrid()
{
	std::cout << "\n\n";
	for (int i = 0; i < gridX; i++)
	{
	       	for (int j = 0; j < gridY; j++)
		{
			for (int k = 0; k < gridZ; k++)
			{
				std::cout << grid[i][j][k];
			}
			std::cout << "\n";
		}
		std::cout << "\n\n\n";
	}
}

//this method initializes the entire voxel grid to a certain value
void VoxelGrid::initializeVoxelGrid(char val)
{
	for (int i = 0; i < gridX; i++)
	{
	       	for (int j = 0; j < gridY; j++)
		{
			for (int k = 0; k < gridZ; k++)
			{
				grid[i][j][k] = val;
			}
		}
	}
}

int VoxelGrid::getXSize()
{
	return gridX;
}

int VoxelGrid::getYSize()
{
	return gridY;
}

int VoxelGrid::getZSize()
{
	return gridZ;
}

//returns the value at a point in the grid
char VoxelGrid::getValue(int x, int y, int z)
{
	//if the requested position is invalid, return 0xff
	if (((x < 0) | (x > gridX)) || ((x < 0) | (x > gridX)) || ((x < 0) | (x > gridX)))
	{
		return 0xff;
	}
	else
	{
		return grid[x][y][z];
	}
}

// void VoxelGrid::makeCircle()
// {
// 	for (int i = 0; i < gridX; i++)
// 	{
// 	       	for (int j = 0; j < gridY; j++)
// 		{
// 			for (int k = 0; k < gridZ; k++)
// 			{
// 				if ((abs(i - j) < 5) || (abs(j - k) < 5) || (abs(i - k) < 5))
// 				{
// 					grid[i][j][k] = 0x01;
// 				}
// 			}
// 		}
// 	}
// }

//pos is the centre of the rectangle, dims are the dimensions from the centre to the edges
void VoxelGrid::makeRectangle(Ogre::Vector3 pos, Ogre::Vector3 dims, bool add)
{
	for (int i = -dims.x; i <= dims.x; i++)
	{
		for (int j = -dims.y; j <= dims.y; j++)
		{
			for (int k = -dims.z; k <= dims.z; k++)
			{
				//check the current voxel is in bounds
				if ((pos.x + i < gridX) &&
				    (pos.x + i >= 0) &&
				    (pos.y + j < gridY) &&
				    (pos.y + j >= 0) &&
				    (pos.z + k < gridZ) &&
				    (pos.z + k >= 0))
				{
					if (add)
					{
						grid[(int)pos.x + i][(int)pos.y + j][(int)pos.z + k] = 0x01;
					}
					else
					{
						grid[(int)pos.x + i][(int)pos.y + j][(int)pos.z + k] = 0x00;
					}
				}
			}
		}
	}
}

//pos is the centre of the ellipsoid, dims are the dimensions from the centre to the edges
void VoxelGrid::makeEllipsoid(Ogre::Vector3 pos, Ogre::Vector3 dims, bool add)
{
	for (int i = -dims.x; i <= dims.x; i++)
	{
		for (int j = -dims.y; j <= dims.y; j++)
		{
			for (int k = -dims.z; k <= dims.z; k++)
			{
				//check the current voxel is in bounds
				if ((pos.x + i < gridX) &&
				    (pos.x + i >= 0) &&
				    (pos.y + j < gridY) &&
				    (pos.y + j >= 0) &&
				    (pos.z + k < gridZ) &&
				    (pos.z + k >= 0))
				{
					if (pow((i) / (float)dims.x, 2) + pow((j) / (double)dims.y, 2) + pow((k) / (double)dims.z, 2) <= 1)
					{
						if (add)
						{
							grid[(int)pos.x + i][(int)pos.y + j][(int)pos.z + k] = 0x01;
						}
						else
						{
							grid[(int)pos.x + i][(int)pos.y + j][(int)pos.z + k] = 0x00;
						}
					}
				}
			}
		}
	}
}

//pos is the centre of the ellipsoid, dims are the dimensions from the centre to the edges.
//orientation is either x, y or z and indicates which axis the cylinder runs parallel to
void VoxelGrid::makeCylinder(Ogre::Vector3 pos, Ogre::Vector3 dims, char orientation, bool add)
{
	for (int i = -dims.x; i <= dims.x; i++)
	{
		for (int j = -dims.y; j <= dims.y; j++)
		{
			for (int k = -dims.z; k <= dims.z; k++)
			{
				//check the current voxel is in bounds
				if ((pos.x + i < gridX) &&
				    (pos.x + i >= 0) &&
				    (pos.y + j < gridY) &&
				    (pos.y + j >= 0) &&
				    (pos.z + k < gridZ) &&
				    (pos.z + k >= 0))
				{
					if ((orientation == 'x') && (pow((j) / (double)dims.y, 2) + pow((k) / (double)dims.z, 2) <= 1))
					{
						if (add)
						{
							grid[(int)pos.x + i][(int)pos.y + j][(int)pos.z + k] = 0x01;
						}
						else
						{
							grid[(int)pos.x + i][(int)pos.y + j][(int)pos.z + k] = 0x00;
						}
					}
					else if ((orientation == 'y') && (pow((i) / (double)dims.x, 2) + pow((k) / (double)dims.z, 2) <= 1))
					{
						if (add)
						{
							grid[(int)pos.x + i][(int)pos.y + j][(int)pos.z + k] = 0x01;
						}
						else
						{
							grid[(int)pos.x + i][(int)pos.y + j][(int)pos.z + k] = 0x00;
						}
					}
					else if ((orientation == 'z') && (pow((j) / (double)dims.y, 2) + pow((i) / (double)dims.x, 2) <= 1))
					{
						if (add)
						{
							grid[(int)pos.x + i][(int)pos.y + j][(int)pos.z + k] = 0x01;
						}
						else
						{
							grid[(int)pos.x + i][(int)pos.y + j][(int)pos.z + k] = 0x00;
						}
					}
				}
			}
		}
	}
}
