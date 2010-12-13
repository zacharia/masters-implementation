/*

*/

#include "VoxelGrid.h"

VoxelGrid::VoxelGrid()
{
	grid = NULL;
	display = NULL;
	object_addition_granularity = 1.0;
	shapes = std::vector<Shape>();
	verbose = false;
	useMarchingCubes = true;
}


VoxelGrid::VoxelGrid(int size)
{
	grid = NULL;
	display = NULL;
	object_addition_granularity = 1.0;
	shapes = std::vector<Shape>();
	makeVoxelGrid(size);
	verbose = false;
	useMarchingCubes = true;
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
std::string VoxelGrid::toString()
{
	assert(grid->size() > 0);

	std::string ret = "";
	Array2D<OCTREE_TYPE> currSlice;
	
	//loop through the whole array
	for (int k = 0; k < grid->size(); ++k)
	{
		currSlice = grid->zSlice(k);
		for (int i = 0; i < grid->size(); ++i)
		{
			for (int j = 0; j < grid->size(); ++j)
			{
				if (currSlice.at(i,j) == EMPTY_VAL)
				{
					ret += "0";
				}
				else
				{
					ret += "1";
				}
			}
			ret += "\n";
		}
		ret += "\n\n";
	}

	return ret;
}


std::string VoxelGrid::arrayToString(OCTREE_TYPE* array, unsigned int size)
{
	assert(size > 0);

	std::string ret = "";
		
	//loop through the whole array
	
	for (int i = 0; i < size; ++i)
	{		
		for (int j = 0; j < size; ++j)
		{
			for (int k = 0; k < size; ++k)			
			{
				if ( array[k * size * size + i * size + j] == EMPTY_VAL)
				{
					ret += "0";
				}
				else
				{
					ret += "1";
				}
			}
			ret += "\n";
		}
		ret += "\n\n";
	}

	return ret;	
}


void VoxelGrid::outputString(std::string str, std::string path)
{
	std::ofstream out(path.c_str());
	if (out)
	{
		out << str;
		out.close();
	}
	else
	{
		std::cerr << "Failed to create output file: " << path << "\n\n";
		exit(1);
	}	
}


int VoxelGrid::getSize()
{
	return grid->size();
}


//returns the value at a point in the grid
OCTREE_TYPE VoxelGrid::getValue(int x, int y, int z)
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
void VoxelGrid::readFromFile(std::string file)
{
	std::ifstream in(file.c_str());
	if (in)
	{
		shapes.clear();
		//these are temporary variables used to store details when reading in from file
		std::string /*temp_name = "", */curr_type = "", temp;
		/*bool temp_additive = true;
		Ogre::Vector3 temp_pos = Ogre::Vector3(0,0,0), temp_extents = Ogre::Vector3(1,1,1);
		Ogre::Matrix3 temp_orientation = Ogre::Matrix3(1,0,0,0,1,0,0,0,1);*/
		Shape temp_shape;
		
		while (!in.eof())
		{
			in >> curr_type >> std::ws;
			if (curr_type == "name")
			{
				in >> temp_shape.type >> std::ws;
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
					temp_shape.additive = true;
				else
					temp_shape.additive = false;
			}
			else if (curr_type == "position")
			{
				double temp_arr[3];
				for (int i = 0; i < 3; ++i)
				{
					in >> temp >> std::ws;
					temp_arr[i] = atof(temp.c_str());
				}
				temp_shape.position = Ogre::Vector3(temp_arr[0], temp_arr[1], temp_arr[2]);
			}
			else if (curr_type == "extents")
			{
				double temp_arr[3];
				for (int i = 0; i < 3; ++i)
				{
					in >> temp >> std::ws;
					temp_arr[i] = atof(temp.c_str());
				}
				temp_shape.extents = Ogre::Vector3(temp_arr[0], temp_arr[1], temp_arr[2]);
			}
			else if (curr_type == "orientation")
			{
				Ogre::Real temp_arr[3][3];
				for (int i = 0; i < 3; ++i)
				{
					for (int j = 0; j < 3; ++j)
					{
						in >> temp >> std::ws;
						if ((i != 3) && (j != 3))
						{
							temp_arr[i][j] = atof(temp.c_str());		
						}						
					}					
				}
				temp_shape.orientation = Ogre::Matrix3(temp_arr);
			}
			else if (curr_type == "#")
			{
				shapes.push_back(temp_shape);
												
				//reset the stuff
				temp_shape.type = curr_type = "";
				temp_shape.additive = true;
				temp_shape.position = Ogre::Vector3(0,0,0);
				temp_shape.extents = Ogre::Vector3(1,1,1);
				temp_shape.orientation = Ogre::Matrix3(1,0,0,0,1,0,0,0,1);
			}
		}
	}
	else
	{
		std::cout << "Error opening file: " << file << "\n";
		std::cout << "Nothing was created." << "\n";
	}
}


void VoxelGrid::getBoundingBoxes()
{
	assert(shapes.size() > 0);
	
	//these store the biggest and smallest corner positions of the whole ship's bounding box.
	bounding_box_min = Ogre::Vector3(MAX_DOUBLE_VAL, MAX_DOUBLE_VAL, MAX_DOUBLE_VAL);
	bounding_box_max = Ogre::Vector3(-MAX_DOUBLE_VAL, -MAX_DOUBLE_VAL, -MAX_DOUBLE_VAL);
	
	for (std::vector<Shape>::iterator s = shapes.begin(); s != shapes.end(); s++)
	{
		//FIXME: add the rest of the types in here as they're added to the program.
		if ( (s->type == "rectangle") ||  (s->type == "circle") || (s->type == "ellipsoid") || (s->type == "cylinder") )
		{
			s->orientation.Orthonormalize();
			Ogre::Vector3 vertex;//vertices[8];
			for (int i = -1; i <= 1; i += 2)
			{
				for (int j = -1; j <= 1; j += 2)
				{
					for (int k = -1; k <= 1; k += 2)
					{
						vertex = s->position + (s->orientation.GetColumn(0) * s->extents.x * i)
							+ (s->orientation.GetColumn(1) * s->extents.y * j)
							+ (s->orientation.GetColumn(2) * s->extents.z * k);

						//double val;
					
						if (vertex.x > bounding_box_max.x)
						{
							bounding_box_max.x = vertex.x;
						}
					
						if (vertex.y > bounding_box_max.y)
						{
							bounding_box_max.y = vertex.y;
						}
					
						if (vertex.z > bounding_box_max.z)
						{
							bounding_box_max.z = vertex.z;
						}
						//now do the mins
					
						if (vertex.x < bounding_box_min.x)
						{
							bounding_box_min.x = vertex.x;
						}
					
						if (vertex.y < bounding_box_min.y)
						{
							bounding_box_min.y = vertex.y;
						}
					
						if (vertex.z < bounding_box_min.z)
						{
							bounding_box_min.z = vertex.z;
						}
					}
				}
			}
		}
	}
}


Ogre::Vector3 VoxelGrid::getBoundingBoxMinCorner()
{
	return bounding_box_min;
}


Ogre::Vector3 VoxelGrid::getBoundingBoxMaxCorner()
{
	return bounding_box_max;
}


void VoxelGrid::createShapes()
{
	assert(shapes.size() > 0);
	unsigned int count = 1;
	
	for (std::vector<Shape>::iterator i = shapes.begin(); i != shapes.end(); i++)
	{
		if (verbose)
		{
			std::cout << "Voxelizing shape " << count << " of " << shapes.size() << ". Size: " << i->extents.x * i->extents.y * i->extents.z << "\n";
			count++;
		}
		//create an object
		if (i->type == "rectangle")
		{
			makeRectangle(i->position, i->extents, i->orientation, i->additive);			
		}
		else if (i->type == "cylinder")
		{
			makeCylinder(i->position, i->extents, i->orientation, i->additive);
		}
		else if (i->type == "circle")
		{
			//FIXME: get a better solution for circles not having all attributes
			makeCircle(i->position, i->extents.x, i->additive);
		}
		else if (i->type == "ellipsoid")
		{
			makeEllipsoid(i->position, i->extents, i->orientation, i->additive);
		}
	}
}


/*
  this method scales the shape currently stored in the shapes vector so that the largest dimension
  is equal to the 
 */
void VoxelGrid::scaleShapes()
{
	//check everything is legit.
	assert(grid->size() > 0);
	assert(shapes.size() > 0);
	assert(bounding_box_min.x < MAX_DOUBLE_VAL);
	assert(bounding_box_max.x > -MAX_DOUBLE_VAL);

	//calculate the size of the bounding box that contains all the shapes
	Ogre::Vector3 size;
	size.x = bounding_box_max.x - bounding_box_min.x;
	size.y = bounding_box_max.y - bounding_box_min.y;
	size.z = bounding_box_max.z - bounding_box_min.z;
	
	//work out which side of the bounding box is largest.
	char biggest_side;
	if ((size.x > size.y) && (size.x > size.z))
	{
		biggest_side = 'x';
	}
	if ((size.y > size.x) && (size.y > size.z))
	{
		biggest_side = 'y';
	}
	if ((size.z > size.x) && (size.z > size.y))
	{
		biggest_side = 'z';
	}

	//then work out, from the largest side, what ratio we'll be scaling by
	double scale_ratio;
	if (biggest_side == 'x')
	{
		scale_ratio = (double)grid->size() / size.x;
	}
	if (biggest_side == 'y')
	{
		scale_ratio = (double)grid->size() / size.y;
	}
	if (biggest_side == 'z')
	{
		scale_ratio = (double)grid->size() / size.z;
	}

	scale_ratio *= 0.95;

	if (verbose)
	{
		std::cout << "scale ratio: " << scale_ratio << "\n"; //TEMP 	
	}	
	
	//then scale everything by that factor
	for (std::vector<Shape>::iterator i = shapes.begin(); i != shapes.end(); i++)
	{
		i->position = i->position * scale_ratio;
		i->extents = i->extents * scale_ratio;
	}

	getBoundingBoxes();
	//then position the scaled stuff so that it's inside the octree space
	for (std::vector<Shape>::iterator i = shapes.begin(); i != shapes.end(); i++)
	{
		i->position = i->position - bounding_box_min + Vector3(2,2,2);
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
	Array2D<OCTREE_TYPE> currSlice;

	//loop through the whole array
	for (int k = 0; k < grid->size(); ++k)
	{
		if (verbose)
		{
			std::cout << "doing slice: " << k << " of " << grid->size() << "\n"; //TEMP 	
		}
		
		currSlice = grid->zSlice(k);
		for (int j = 0; j < grid->size(); ++j)
		{
			for (int i = 0; i < grid->size(); ++i)
			{
				//if a voxel is 'on'
				if (currSlice.at(j,i) == OCCUPIED_VAL)
				{
#ifdef HIDE_VOXELS
					//then check if it's actually visible
					//this is a very cheap conservative hack at the moment
					if (((k == grid->size() - 1) ||
					    (k == 0) ||
					    (j == grid->size() - 1) ||
					    (j == 0) ||
					    (i == grid->size() - 1) ||
					    (i == 0))
					    ||
					    //actual visibility check
					    ((grid->at(i+1,j,k) == EMPTY_VAL) ||
					    (grid->at(i-1,j,k) == EMPTY_VAL) ||
					    (grid->at(i,j+1,k) == EMPTY_VAL) ||
					    (grid->at(i,j-1,k) == EMPTY_VAL) ||
					    (grid->at(i,j,k+1) == EMPTY_VAL) ||
					    (grid->at(i,j,k-1) == EMPTY_VAL))

					    /*!((grid->at(i+1,j,k) == OCCUPIED_VAL) &&
					     (grid->at(i-1,j,k) == OCCUPIED_VAL) &&
					     (grid->at(i,j+1,k) == OCCUPIED_VAL) &&
					     (grid->at(i,j-1,k) == OCCUPIED_VAL) &&
					     (grid->at(i,j,k+1) == OCCUPIED_VAL) &&
					     (grid->at(i,j,k-1) == OCCUPIED_VAL))*/
						)
#endif
					{						
						display->addVoxelBillboard(Ogre::Vector3(i,j,k));	
					}					
				}
			}
		}
	}
}


void VoxelGrid::polygonize()
{
	//do some sanity checks
	assert(display != NULL);
	assert(grid->size() > 0);

	//make a slice for use in slicing stuff
	Array2D<OCTREE_TYPE> currSlice;
	TriangleMesh output_mesh;
	int temp_size = grid->size();
	//FIXME: this might not be big enough for very large arrays?
	MeshExtractor extractor;
	extractor.setOctree(grid);
	extractor.setVerbose(verbose);

	output_mesh = polygonizeBlock(grid->size());
		
	display->createTriangleMesh("ship");
	display->addToTriangleMesh("ship", &output_mesh, true);	
}


TriangleMesh VoxelGrid::polygonizeBlock(unsigned int size, Ogre::Vector3 position)
{
	assert(grid->size() > 0);
	assert(size <= grid->size());
	assert(position.x + size <= grid->size());
	assert(position.y + size <= grid->size());
	assert(position.z + size <= grid->size());
	
	TriangleMesh ret;	
	MeshExtractor extractor;

	extractor.setOctree(grid);
	extractor.setVerbose(verbose);

	//make a dummy array that doesn't actually contain the grid, but just keeps the extractor methods happy.
	//this will crash if used, but since the extractor has been changed to only use the octree this won't
	//be a problem.
	OCTREE_TYPE* input_grid = new OCTREE_TYPE[10];

	if (useMarchingCubes)
	{
		extractor.extractMeshWithMarchingCubes(input_grid, size, size, size, BOUNDARY_VAL, &ret);	
	}
	else
	{
		extractor.extractMeshWithMarchingTetrahedra(input_grid, size, size, size, BOUNDARY_VAL, &ret);
	}
	
	delete [] input_grid;
	return ret;
}


void VoxelGrid::makeCircle(Ogre::Vector3 pos, int radius, bool add)
{
	assert(radius >= 1);
	
	Ogre::Vector3 currpos;
	
	for (double i = -radius; i <= radius; i += object_addition_granularity)
	{		
		for (double j = -radius; j <= radius; j += object_addition_granularity)
		{
			for (double k = -radius; k <= radius; k += object_addition_granularity)
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
	
	for (double i = -extents.x; i <= extents.x; i += object_addition_granularity)
	{		
		for (double j = -extents.y; j <= extents.y; j += object_addition_granularity)
		{
			for (double k = -extents.z; k <= extents.z; k += object_addition_granularity)
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
	
	for (double i = -extents.x; i <= extents.x; i += object_addition_granularity)
	{		
		for (double j = -extents.y; j <= extents.y; j += object_addition_granularity)
		{
			for (double k = -extents.z; k <= extents.z; k += object_addition_granularity)
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
	
	for (double i = -extents.x; i <= extents.x; i += object_addition_granularity)
	{
		for (double j = -extents.y; j <= extents.y; j += object_addition_granularity)
		{
			for (double k = -extents.z; k <= extents.z; k += object_addition_granularity)
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


void VoxelGrid::setAdditionGranularity(double g)
{
	object_addition_granularity = g;
}


void VoxelGrid::setVerbose(bool v)
{
	verbose = v;
}


void VoxelGrid::setUseCubes(bool v)
{
	useMarchingCubes = v;
}
