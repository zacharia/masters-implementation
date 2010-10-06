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
	grid = new OCTREE_DEF(size, EMPTY_VAL);
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
		return EMPTY_VAL;
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
		//these are temporary variables used to store details when reading in from file
		std::string temp_name = "", curr_type = "", temp;
		bool temp_additive = true;
		Ogre::Vector3 temp_pos = Ogre::Vector3(0,0,0), temp_extents = Ogre::Vector3(1,1,1);
		Ogre::Matrix3 temp_orientation = Ogre::Matrix3(1,0,0,0,1,0,0,0,1);
		
		while (!in.eof())
		{
			in >> curr_type >> std::ws;
			if (curr_type == "name")
			{
				in >> temp_name >> std::ws;
			}
			else if (curr_type == "active")
			{
				//this tag should never appear in the output. Why am I bothering with it?
				in >> temp >> std::ws;
				if (temp == "True")
				{					
				}
				else
				{					
				}
			}
			else if (curr_type == "additive")
			{
				in >> temp >> std::ws;
				if (temp == "True")
					temp_additive = true;
				else
					temp_additive = false;
			}
			else if (curr_type == "position")
			{
				double temp_arr[3];
				for (int i = 0; i < 3; ++i)
				{
					in >> temp >> std::ws;
					temp_arr[i] = atof(temp.c_str());
				}
				temp_pos = Ogre::Vector3(temp_arr[0], temp_arr[1], temp_arr[2]);
			}
			else if (curr_type == "extents")
			{
				double temp_arr[3];
				for (int i = 0; i < 3; ++i)
				{
					in >> temp >> std::ws;
					temp_arr[i] = atof(temp.c_str());
				}
				temp_extents = Ogre::Vector3(temp_arr[0], temp_arr[1], temp_arr[2]);
			}
			else if (curr_type == "orientation")
			{
				Ogre::Real temp_arr[3][3];
				for (int i = 0; i < 4; ++i)
				{
					for (int j = 0; j < 4; ++j)
					{
						in >> temp >> std::ws;
						if ((i != 3) && (j != 3))
						{
							temp_arr[i][j] = atof(temp.c_str());		
						}						
					}					
				}
				temp_orientation = Ogre::Matrix3(temp_arr);
			}
			else if (curr_type == "#")
			{
				//create an object
				if (temp_name == "rectangle")
				{
					makeRectangle(temp_pos, temp_extents, temp_orientation, temp_additive);
					std::cout << temp_name << " " << temp_additive << " " << temp_pos.x << " " << temp_pos.y << " " << temp_pos.z << "\n";
				}
				else if (temp_name == "cylinder")
				{
					makeCylinder(temp_pos, temp_extents, temp_orientation, temp_additive);
				}
				else if (temp_name == "circle")
				{
					//FIXME: get a better solution for circles not having all attributes
					makeCircle(temp_pos, temp_extents.x, temp_additive);
				}
				else if (temp_name == "ellipsoid")
				{
					makeEllipsoid(temp_pos, temp_extents, temp_orientation, temp_additive);
				}
								
				//reset the stuff
				temp_name = curr_type = "";
				temp_additive = true;
				temp_pos = Ogre::Vector3(0,0,0);
				temp_extents = Ogre::Vector3(1,1,1);
				temp_orientation = Ogre::Matrix3(1,0,0,0,1,0,0,0,1);
			}
		}
	}
	else
	{
		std::cout << "Error openinyg file: " << file << "\n";
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
	for (int k = 0; k < grid->size(); ++k)
	{
		//std::cout << "doing slice: " << i << "\n"; //TEMP 
		currSlice = grid->zSlice(k);
		for (int j = 0; j < grid->size(); ++j)
		{
			for (int i = 0; i < grid->size(); ++i)
			{
				//if a voxel is 'on'
				if (currSlice.at(j,i) == OCCUPIED_VAL)
				{
					//then check if it's actually visible
					//this is a very cheap conservative hack at the moment
					if ((k == grid->size() - 1) ||
					    (k == 0) ||
					    (j == grid->size() - 1) ||
					    (j == 0) ||
					    (i == grid->size() - 1) ||
					    (i == 0) ||
					    //actual visibility check
					    (grid->at(i+1,j,k) == EMPTY_VAL) ||
					    (grid->at(i-1,j,k) == EMPTY_VAL) ||
					    (grid->at(i,j+1,k) == EMPTY_VAL) ||
					    (grid->at(i,j-1,k) == EMPTY_VAL) ||
					    (grid->at(i,j,k+1) == EMPTY_VAL) ||
					    (grid->at(i,j,k-1) == EMPTY_VAL) )
					{						
						display->addVoxelBillboard(Ogre::Vector3(i,j,k));	
					}					
				}
			}
		}
	}
}

void VoxelGrid::makeCircle(Ogre::Vector3 pos, int radius, bool add)
{
	assert(radius >= 1);
	
	Ogre::Vector3 currpos;
		
	for (int i = -radius; i <= radius; i++)
	{		
		for (int j = -radius; j <= radius; j++)
		{
			for (int k = -radius; k <= radius; k++)
			{
				currpos = pos + Ogre::Vector3(i,j,k);
				//check the current voxel is in bounds
				if ((currpos.x < grid->size()) &&
				    (currpos.x >= 0) &&
				    (currpos.y < grid->size()) &&
				    (currpos.y >= 0) &&
				    (currpos.z < grid->size()) &&
				    (currpos.z >= 0))
				{					
					if (pow(i , 2) + pow(j, 2) + pow(k, 2) <= pow(radius,2))
					{
						if (add)
						{
							grid->set(currpos.x, currpos.y, currpos.z, OCCUPIED_VAL);
						}
						else
						{
							grid->set(currpos.x, currpos.y, currpos.z, EMPTY_VAL);
						}
					}
				}
			}
		}
	}
}

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
						grid->set(currpos.x, currpos.y, currpos.z, OCCUPIED_VAL);
					}
					else
					{
						grid->set(currpos.x, currpos.y, currpos.z, EMPTY_VAL);
					}
				}
			}
		}
	}
}

//pos is the centre of the ellipsoid, extents are the dimensions from the centre to the edges
void VoxelGrid::makeEllipsoid(Ogre::Vector3 pos, Ogre::Vector3 extents, Ogre::Matrix3 orientation, bool add)
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
					if (pow((i) / (float)extents.x, 2) + pow((j) / (double)extents.y, 2) + pow((k) / (double)extents.z, 2) <= 1)
					{
						if (add)
						{
							grid->set(currpos.x, currpos.y, currpos.z, OCCUPIED_VAL);
						}
						else
						{
							grid->set(currpos.x, currpos.y, currpos.z, EMPTY_VAL);
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
					if (pow((i) / (double)extents.x, 2) + pow((k) / (double)extents.z, 2) <= 1)
					{
						if (add)
						{
							grid->set(currpos.x, currpos.y, currpos.z, OCCUPIED_VAL);
						}
						else
						{
							grid->set(currpos.x, currpos.y, currpos.z, EMPTY_VAL);
						}
					}
				}
			}
		}
	}
}
