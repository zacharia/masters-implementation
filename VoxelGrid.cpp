/*

*/

#include "VoxelGrid.h"

VoxelGrid::VoxelGrid()
{
	grid = NULL;
	display = NULL;
}

VoxelGrid::VoxelGrid(int size)
{
	grid = NULL;
	display = NULL;
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
	std::cout << "\nTODO\n";
	//TODO: this needs to be redone, if it's even needed at all
}

int VoxelGrid::getSize()
{
	return grid->size();
}

//returns the value at a point in the grid
unsigned int VoxelGrid::getValue(int x, int y, int z)
{
	//if the requested position is invalid, return 0xff
	if ((x < 0) || (y < 0) || (z < 0) || (x > grid->size()) || (y > grid->size()) || (z > grid->size()))
	{
		return 0;
	}
	else
	{
		return grid->at(x,y,z);
	}
}

/*
  This reads the file given as an argument (which should be produced by the python interpreter)
  and creates the contents of that file in the voxel grid.
 */
void VoxelGrid::createFromFile(std::string file)
{
	std::ifstream in(file.c_str());
	if (in)
	{
		/*while (!in.eof())
		{
			
		}*/
	}
	else
	{
		std::cout << "Error opening file: " << file << "\n";
		std::cout << "Nothing was created." << "\n";
	}
}

//gives a pointer to the display object that is used to display stuff
void VoxelGrid::setDisplay(OgreDisplay* d)
{
	display = d;
}

//this updates the display object given to display the voxel grid as it currently is.
void VoxelGrid::updateDisplay()
{
	//do some sanity checks
	assert(display != NULL);
	assert(grid->size() > 0);

	//make a slice for use in slicing stuff
	Array2D<unsigned int> currSlice;

	//loop through the whole array
	for (int i = 0; i < grid->size(); ++i)
	{
		std::cout << "doing slice: " << i << "\n"; //TEMP 
		currSlice = grid->zSlice(i);
		for (int j = 0; j < grid->size(); ++j)
		{
			for (int k = 0; k < grid->size(); ++k)
			{
				//if a voxel is 'on'
				if (currSlice.at(j,k) == 0xffffffff)
				{
					//then check if it's actually visible
					//this is a very cheap conservative hack currently used
					if ((grid->at(i+1,j,k) == 0) ||
					    (grid->at(i-1,j,k) == 0) ||
					    (grid->at(i,j+1,k) == 0) ||
					    (grid->at(i,j-1,k) == 0) ||
					    (grid->at(i,j,k+1) == 0) ||
					    (grid->at(i,j,k-1) == 0) )					    
					{
						display->addVoxelBillboard(Ogre::Vector3(i,j,k));	
					}					
				}
			}
		}
	}
}

// void VoxelGrid::makeCircle()
// {
// 	for (int i = 0; i < grid->size(); i++)
// 	{
// 	       	for (int j = 0; j < grid->size(); j++)
// 		{
// 			for (int k = 0; k < grid->size(); k++)
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
	Ogre::Vector3 currpos;
	orientation.Orthonormalize();
	
	for (int i = -extents.x; i <= extents.x; i++)
	{		
		for (int j = -extents.y; j <= extents.y; j++)
		{
			for (int k = -extents.z; k <= extents.z; k++)
			{
				currpos = pos + (orientation.GetColumn(0) * i) +
					(orientation.GetColumn(1) * j) +
					(orientation.GetColumn(2) * k);
				//check the current voxel is in bounds
				if ((currpos.x < grid->size()) &&
				    (currpos.x >= 0) &&
				    (currpos.y < grid->size()) &&
				    (currpos.y >= 0) &&
				    (currpos.z < grid->size()) &&
				    (currpos.z >= 0))
				{
					if (add)
					{
						grid->set(currpos.x, currpos.y, currpos.z, 0xffffffff);
					}
					else
					{
						grid->set(currpos.x, currpos.y, currpos.z, 0);
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
				if ((pos.x + i < grid->size()) &&
				    (pos.x + i >= 0) &&
				    (pos.y + j < grid->size()) &&
				    (pos.y + j >= 0) &&
				    (pos.z + k < grid->size()) &&
				    (pos.z + k >= 0))
				{
					if (pow((i) / (float)extents.x, 2) + pow((j) / (double)extents.y, 2) + pow((k) / (double)extents.z, 2) <= 1)
					{
						if (add)
						{
							//grid->size()(int)pos.x + i][(int)pos.y + j][(int)pos.z + k] = 0x01;
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
