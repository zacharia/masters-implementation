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
	usePriorities = true;
	automata_rules_file = "";
	automata_rules_method = "";
	automata_num_iterations = -1;
	automata_neighbourhood_size = -1;
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
	usePriorities = true;
	automata_rules_file = "";
	automata_rules_method = "";
	automata_num_iterations = -1;
	automata_neighbourhood_size = -1;
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
	grid = new Octree(size);
}


//this method displays the voxel grid on the command line
std::string VoxelGrid::toString()
{
	assert(grid->getSize() > 0);

	return grid->printTree();
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
	return grid->getSize();
}


//returns the value at a point in the grid
SPACE_TYPE VoxelGrid::getValue(int x, int y, int z)
{
	//if the requested position is invalid, return 0xff
	if ((x < 0) || (y < 0) || (z < 0) || (x > grid->getSize()) || (y > grid->getSize()) || (z > grid->getSize()))
	{
		return SPACE_EMPTY;
	}
	else
	{
		return grid->at(x,y,z).solid;
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
		std::string curr_type = "", temp;

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
			else if (curr_type == "priority")
			{
				in >> temp >> std::ws;
				temp_shape.priority = atoi(temp.c_str());
			}
			else if (curr_type == "tags_begin")
			{
				in >> temp >> std::ws;
				while (temp != "tags_end")
				{
					temp_shape.tags.insert(temp);
					in >> temp >> std::ws;
				}
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
				temp_shape.tags = std::set<std::string>();
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
		if ( ( (s->type == "rectangle") ||  (s->type == "circle") || (s->type == "ellipsoid") || (s->type == "cylinder") ) && (s->additive) )
		{
			s->orientation.Orthonormalize();
			Ogre::Vector3 vertex;
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

	if (usePriorities)
	{
		std::priority_queue<Shape, std::vector<Shape>, ShapeComparisionObject> shapes_priority_queue;
		for (std::vector<Shape>::iterator i = shapes.begin(); i != shapes.end(); i++)
		{
			shapes_priority_queue.push(*i);
		}
		
		while (!shapes_priority_queue.empty())
		{
			Shape queue_top = shapes_priority_queue.top();
			
			if (verbose)
			{
				std::cout << "Voxelizing shape. " << shapes_priority_queue.size() << " remain. Size: " << queue_top.extents.x * queue_top.extents.y * queue_top.extents.z << "\n";
			}
			
			//create an object
			if (queue_top.type == "rectangle")
			{
				makeRectangle(queue_top.position, queue_top.extents, queue_top.orientation, queue_top.additive, queue_top.tags);			
			}
			else if (queue_top.type == "cylinder")
			{
				makeCylinder(queue_top.position, queue_top.extents, queue_top.orientation, queue_top.additive, queue_top.tags);
			}
			else if (queue_top.type == "circle")
			{
				//FIXME: get a better solution for circles not having all attributes
				makeCircle(queue_top.position, queue_top.extents.x, queue_top.additive, queue_top.tags);
			}
			else if (queue_top.type == "ellipsoid")
			{
				makeEllipsoid(queue_top.position, queue_top.extents, queue_top.orientation, queue_top.additive, queue_top.tags);
			}

			shapes_priority_queue.pop();
		}
	}
	else
	{
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
				makeRectangle(i->position, i->extents, i->orientation, i->additive, i->tags);
			}
			else if (i->type == "cylinder")
			{
				makeCylinder(i->position, i->extents, i->orientation, i->additive, i->tags);
			}
			else if (i->type == "circle")
			{
				//FIXME: get a better solution for circles not having all attributes
				makeCircle(i->position, i->extents.x, i->additive, i->tags);
			}
			else if (i->type == "ellipsoid")
			{
				makeEllipsoid(i->position, i->extents, i->orientation, i->additive, i->tags);
			}
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
	assert(grid->getSize() > 0);
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
		scale_ratio = (double)grid->getSize() / size.x;
	}
	if (biggest_side == 'y')
	{
		scale_ratio = (double)grid->getSize() / size.y;
	}
	if (biggest_side == 'z')
	{
		scale_ratio = (double)grid->getSize() / size.z;
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
	assert(grid->getSize() > 0);

	//loop through the whole array
	for (int k = 0; k < grid->getSize(); ++k)
	{
		if (verbose)
		{
			std::cout << "doing slice: " << k << " of " << grid->getSize() << "\n"; //TEMP 	
		}
		
		for (int j = 0; j < grid->getSize(); ++j)
		{
			for (int i = 0; i < grid->getSize(); ++i)
			{
				//if a voxel is 'on'
				if (grid->at(i,j,k).solid == SPACE_SOLID)
				{
#ifdef HIDE_VOXELS
					//then check if it's actually visible
					//this is a very cheap conservative hack at the moment
					if (((k == grid->getSize() - 1) ||
					    (k == 0) ||
					    (j == grid->getSize() - 1) ||
					    (j == 0) ||
					    (i == grid->getSize() - 1) ||
					    (i == 0))
					    ||
					    //actual visibility check
					    ((grid->at(i+1,j,k).solid == SPACE_EMPTY) ||
					    (grid->at(i-1,j,k).solid == SPACE_EMPTY) ||
					    (grid->at(i,j+1,k).solid == SPACE_EMPTY) ||
					    (grid->at(i,j-1,k).solid == SPACE_EMPTY) ||
					    (grid->at(i,j,k+1).solid) == SPACE_EMPTY) ||
					    (grid->at(i,j,k-1).solid == SPACE_EMPTY))
#endif
					{
						//This line is for doing each voxel as a point
						//display->addVoxelBillboard(Ogre::Vector3(i,j,k));
						
						//This line is for doing each as a cube.
						display->addCube(Ogre::Vector3(i,j,k));	
					}					
				}
			}
		}
	}
}


//this method calls all the stuff to make the octree aggregate and detail nodes.
void VoxelGrid::doSurfaceDetail()
{
	//if we don't have a rule set to use, then just return from this method.
	if (automata_rules_file == "")
	{
		std::cout << "warning: no automata rule file specified. No detailing will be done." << "\n";
		return;
	}
	
	assert(grid != NULL);

	//get the surface voxels from the octree.
	std::set<Ogre::Vector3, VectorLessThanComparator> surface_voxels = grid->getSurfaceVoxels(26, 0);

	//make a new empty octree
	Octree* surface_voxels_only_octree = new Octree(grid->getSize());

	//add the surface voxels to the new octree.
	VoxelInformation temp;
	for (std::set<Ogre::Vector3, VectorLessThanComparator>::iterator i = surface_voxels.begin(); i != surface_voxels.end(); i++)
	{
		//get the VoxelInformation for each surface voxel from the main octree.
		temp = grid->at(i->x, i->y, i->z);

		//calculate the normal for that voxel (we only add these to the surface voxels octree to save space)
		temp.aggregate_normal.x = grid->at(i->x - 1, i->y, i->z).solid - grid->at(i->x + 1, i->y, i->z).solid;
		temp.aggregate_normal.y = grid->at(i->x, i->y - 1, i->z).solid - grid->at(i->x, i->y + 1, i->z).solid;
		temp.aggregate_normal.z = grid->at(i->x, i->y, i->z - 1).solid - grid->at(i->x, i->y, i->z + 1).solid;
		temp.aggregate_normal.normalise();

		//now add that voxel's information to the surface voxels only octree.
		surface_voxels_only_octree->set(i->x, i->y, i->z, temp);
	}

	//now aggregate the surface voxels octree.
	surface_voxels_only_octree->makeAggregateInformation();

	//then run the cellular automata on the aggregated tree.
	surface_voxels_only_octree->runAutomataRules(automata_rules_file, automata_rules_method, automata_num_iterations, automata_neighbourhood_size);

	//now take the detailing information from the surface voxels tree and put it back into the main octree.
	std::set<Ogre::Vector3, VectorLessThanComparator> detailed_surface_voxels = grid->getAllSolidVoxels();
	for (std::set<Ogre::Vector3, VectorLessThanComparator>::iterator i = detailed_surface_voxels.begin(); i != detailed_surface_voxels.end(); i++)
	{
		VoxelInformation temp = grid->at(i->x, i->y, i->z);
		temp.detail_info = surface_voxels_only_octree->at(i->x, i->y, i->z).detail_info;
		grid->set(i->x, i->y, i->z, temp);
	}

	//delete the surface voxels only octree now that we're done with it.
	//delete surface_voxels_only_octree;

	//the code from here to the end of the method is all for making sure that all voxels are detailed properly.
	//The reason it's done here is to speed up the second pass at making all the voxels detailed, which is done in the MeshGenerator class in vMarch.
	int count = 0;
	if (verbose)
	{
		std::cout << "Ensuring neighbouring undetailed voxels gets properly detailed." << "\n";	
	}
	
	int border_size = 2;
	//get the surface voxels, with a border of 2 empty voxels around them
	surface_voxels = grid->getSurfaceVoxels(26, border_size);
	//loop over the surface voxels (and their border)
	for (std::set<Ogre::Vector3, VectorLessThanComparator>::iterator i = surface_voxels.begin(); i != surface_voxels.end(); i++)
	{
		if (verbose)
		{
			count++;
			if (count % 1000 == 0)
			{
				std::cout << count << " of " << surface_voxels.size() << "\n"; //TEMP 	
			}
		}

		//get the current surface voxel and make some temp variables.
		VoxelInformation* curr_voxel_info = grid->at_pointer(i->x, i->y, i->z), *temp = NULL, *min_detail_info = NULL;
		//if the current voxel is not detailed
		if ((curr_voxel_info != NULL) && (curr_voxel_info->detail_info.empty()))
		{
			//then look for it's closest detailed neighbour in a cubic neighbourhood of size border_size * 2
			double min_dist = 99999;
			for (int x = -border_size; x <= border_size; ++x)
				for (int y = -border_size; y <= border_size; ++y)
					for (int z = -border_size; z <= border_size; ++z)
					{
						temp = surface_voxels_only_octree->at_pointer(i->x + x, i->y + y, i->z + z);
						if ((temp != NULL) && !(temp->detail_info.empty()))
						{
							double dist = i->distance(Ogre::Vector3(i->x + x, i->y + y, i->z + z));
							if (dist < min_dist)
							{
								min_dist = dist;
								min_detail_info = temp;
							}
						}
					}

			//copy the detailing of the closest detailed neighbour into the current voxel.
			if (min_dist < sqrt(border_size * border_size * border_size))
			{
				//this is how I used to do it. It gave full coverage, but would assign detailing incorrectly to some of the voxels.
				//grid->at_pointer(i->x, i->y, i->z)->detail_info = min_detail_info->detail_info;

				//this is the new way. It doesn't give full coverage (that's done in MeshGenerator, in vMarch (if (detail_info.empty()...), but those that it does assign detailing info to are all done correctly.
				VoxelInformation empty_space = *curr_voxel_info;
				empty_space.detail_info = min_detail_info->detail_info;
				grid->set(i->x, i->y, i->z, empty_space);
			}			
		}
	}

	delete surface_voxels_only_octree;
}


void VoxelGrid::polygonize()
{
	//do some sanity checks
	assert(display != NULL);
	assert(grid->getSize() > 0);
	
	MeshGenerator mesh_generator;
	mesh_generator.setVerbose(verbose);
	mesh_generator.setOctree(grid);
	mesh_generator.setOgreDisplay(display);
	mesh_generator.setNodeSizeRestriction(1);
	if (useMarchingCubes)
	{
		mesh_generator.vMarchingCubes();
	}
	else
	{
		mesh_generator.vMarchingTetrahedrons();
	}
}


void VoxelGrid::makeCircle(Ogre::Vector3 pos, int radius, bool add, std::set<std::string> in_tags)
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
				if ((currpos.x < grid->getSize()) &&
				    (currpos.x >= 0) &&
				    (currpos.y < grid->getSize()) &&
				    (currpos.y >= 0) &&
				    (currpos.z < grid->getSize()) &&
				    (currpos.z >= 0))
				{					
					if (pow(i , 2) + pow(j, 2) + pow(k, 2) <= pow(radius,2))
					{
						if (add)
						{
							grid->set(currpos.x, currpos.y, currpos.z, VoxelInformation(SPACE_SOLID, in_tags));
						}
						else
						{
							grid->set(currpos.x, currpos.y, currpos.z, VoxelInformation(SPACE_EMPTY, in_tags));
						}
					}
				}
			}
		}
	}
}


//pos is the centre of the rectangle, extents are the dimensions from the centre to the edges
void VoxelGrid::makeRectangle(Ogre::Vector3 pos, Ogre::Vector3 extents, Ogre::Matrix3 orientation, bool add, std::set<std::string> in_tags)
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
				if ((currpos.x < grid->getSize()) &&
				    (currpos.x >= 0) &&
				    (currpos.y < grid->getSize()) &&
				    (currpos.y >= 0) &&
				    (currpos.z < grid->getSize()) &&
				    (currpos.z >= 0))
				{
					if (add)
					{
						grid->set(currpos.x, currpos.y, currpos.z, VoxelInformation(SPACE_SOLID, in_tags));
					}
					else
					{
						grid->set(currpos.x, currpos.y, currpos.z, VoxelInformation(SPACE_EMPTY, in_tags));
					}
				}
			}
		}
	}
}


//pos is the centre of the ellipsoid, extents are the dimensions from the centre to the edges
void VoxelGrid::makeEllipsoid(Ogre::Vector3 pos, Ogre::Vector3 extents, Ogre::Matrix3 orientation, bool add, std::set<std::string> in_tags)
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
				if ((currpos.x < grid->getSize()) &&
				    (currpos.x >= 0) &&
				    (currpos.y < grid->getSize()) &&
				    (currpos.y >= 0) &&
				    (currpos.z < grid->getSize()) &&
				    (currpos.z >= 0))
				{
					if (pow((i) / (float)extents.x, 2) + pow((j) / (double)extents.y, 2) + pow((k) / (double)extents.z, 2) <= 1)
					{
						if (add)
						{
							grid->set(currpos.x, currpos.y, currpos.z, VoxelInformation(SPACE_SOLID, in_tags));
						}
						else
						{
							grid->set(currpos.x, currpos.y, currpos.z, VoxelInformation(SPACE_EMPTY, in_tags));
						}
					}
				}
			}
		}
	}
}


//pos is the centre of the ellipsoid, extents are the dimensions from the centre to the edges.
//orientation is either x, y or z and indicates which axis the cylinder runs parallel to
void VoxelGrid::makeCylinder(Ogre::Vector3 pos, Ogre::Vector3 extents, Ogre::Matrix3 orientation, bool add, std::set<std::string> in_tags)
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
				if ((currpos.x < grid->getSize()) &&
				    (currpos.x >= 0) &&
				    (currpos.y < grid->getSize()) &&
				    (currpos.y >= 0) &&
				    (currpos.z < grid->getSize()) &&
				    (currpos.z >= 0))
				{
					if (pow((i) / (double)extents.x, 2) + pow((k) / (double)extents.z, 2) <= 1)
					{
						if (add)
						{
							grid->set(currpos.x, currpos.y, currpos.z, VoxelInformation(SPACE_SOLID, in_tags));
						}
						else
						{
							grid->set(currpos.x, currpos.y, currpos.z, VoxelInformation(SPACE_EMPTY, in_tags));
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


void VoxelGrid::setUsePriorities(bool v)
{
	usePriorities = v;
}


void VoxelGrid::setAutomataRuleSet(std::string file)
{
	automata_rules_file = file;
}

void VoxelGrid::setAutomataRuleMethod(std::string in)
{
	automata_rules_method = in;
}

void VoxelGrid::setAutomataIterations(int in)
{
	automata_num_iterations = in;
}

void VoxelGrid::setAutomataNeighbourhoodSize(int in)
{
	automata_neighbourhood_size = in;
}

