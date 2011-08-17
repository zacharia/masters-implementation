/*
  Octree.cpp
 */

#include "Octree.h"

//===================Octree Method implementations=========================

//default constructor. makes octree of size 256
Octree::Octree()
{
	root = NULL;
	this->createOctree(256);
}


//copy constructor
Octree::Octree(const Octree& copy_from_me)
{
	root = new OctreeNode(*(copy_from_me.root));
	this->size = copy_from_me.size;
	this->auto_optimize_on = copy_from_me.auto_optimize_on;
	this->auto_optimize_interval = copy_from_me.auto_optimize_interval;
	this->auto_optimize_counter = copy_from_me.auto_optimize_counter;
}


//parameterized constructor, makes octree of size equal to argument's value
Octree::Octree(int in_size)
{
	root = NULL;
	this->createOctree(in_size);
}


//this method does the creation legwork of the class. It's what the
//constructors call to actually do the creation stuff.
void Octree::createOctree(int in_size)
{
	//destroy any existing octree before we make a new one
	if (root != NULL)
	{
		delete root;
		this->root = NULL;	
	}
		
	//set the size
	this->setSize(in_size);

	//make a default VoxelInformation Object to initialize the octree to (empty)
	VoxelInformation temp_info;

	//create the root node.
	this->root = new OctreeNode(this->getSize(), temp_info);

	//set up the default auto_optimize values.
	this->auto_optimize_on = true;
	this->auto_optimize_interval = in_size * in_size;
	this->auto_optimize_counter = 0;
}


//destructor. deletes the octree.
Octree::~Octree()
{
	//if the octree doesn't actually need any destroying, do nothing
	if (root == NULL)
	{
		this->size = 0;
		return;
	}
	//if it does need destroy, do the stuff.
	else
	{
		delete this->root;
		this->root = NULL;
		this->size = 0;
	}
}


void Octree::setSize(int in)
{
	//check the size is a valid value, both positive and a power of 2
	assert(in > 0);
	assert( ((in - 1) & in) == 0 );

	//check that the tree is empty.
	assert(root == NULL);
	
	//change the size of the octree
	this->size = in;
}


int Octree::getSize()
{
	return this->size;
}


VoxelInformation Octree::at(int x, int y, int z, int depth_restriction)
{
	// I commented these out in favour of just returning an empty value when asking for something outside the octree
	// assert(x >= 0 && x < this->size);
	// assert(y >= 0 && y < this->size);
	// assert(z >= 0 && z < this->size);

	if ( (x < 0) || (x > this->size) || (y < 0) || (y > this->size) || (z < 0) || (z > this->size) )
	{
		return VoxelInformation(SPACE_EMPTY);
	}

	//TODO: code an iterative way of doing this method, for
	//efficiency. This current way is because it's more
	//understandable, but the function calls make it less
	//efficient.
	return root->at(x,y,z, this->size, depth_restriction);
}


VoxelInformation* Octree::at_pointer(int x, int y, int z, int depth_restriction)
{
	if ( (x < 0) || (x > this->size) || (y < 0) || (y > this->size) || (z < 0) || (z > this->size) )
	{
		return NULL;
	}
	
	//TODO: code an iterative way of doing this method, for
	//efficiency. This current way is because it's more
	//understandable, but the function calls make it less
	//efficient.
	return root->at_pointer(x,y,z, this->size, depth_restriction);
}


void Octree::set(int x, int y, int z, VoxelInformation value)
{
	assert(x >= 0 && x < this->size);
	assert(y >= 0 && y < this->size);
	assert(z >= 0 && z < this->size);

	//TODO: iterative way of doing this, instead of recursive.
	root->set(x,y,z, value, this->size);

	//this code is so that the tree is optimized every auto_optimize_interval sets/erasures.
	this->auto_optimize_counter++;
	if ((this->auto_optimize_on) && (this->auto_optimize_counter >= this->auto_optimize_interval))
	{
		this->optimizeTree();
		this->auto_optimize_counter = 0;
	}
}


//This method sets all values of an axis-aligned rectangle in the
//octree, smallest and largest corners given as args, to the value
//given as an arg.
void Octree::setRange(Ogre::Vector3 lower, Ogre::Vector3 upper, VoxelInformation value)
{
	root->setRange(lower, upper, Ogre::Vector3(this->size / 2), value);
}


//This method erases a value from the tree.
void Octree::erase(int x, int y, int z)
{
	assert(x >= 0 && x < this->size);
	assert(y >= 0 && y < this->size);
	assert(z >= 0 && z < this->size);

	//TODO: iterative way of doing this.
	root->erase(x,y,z, this->size);

	//this code is so that the tree is optimized every auto_optimize_interval sets/erasures.
	this->auto_optimize_counter++;
	if ((this->auto_optimize_on) && (this->auto_optimize_counter >= this->auto_optimize_interval))
	{
		this->optimizeTree();
		this->auto_optimize_counter = 0;
	}
}


//This method erases all values contained in an axis-aligned rectangle
void Octree::eraseRange(Ogre::Vector3 lower, Ogre::Vector3 upper)
{
	root->eraseRange(lower, upper, Ogre::Vector3(this->size / 2));
}


//this method optimizes the tree of unnecessary nodes.
void Octree::optimizeTree()
{
	root->optimizeNode();
}


//this method returns a string representation of the tree.
std::string Octree::printTree()
{
	return root->printNode();
}


//This method goes through the tree and populates non-leaf nodes with their aggregate information.
void Octree::makeAggregateInformation()
{
	root->makeAggregateInformation();
}

	
std::set<Ogre::Vector3, VectorLessThanComparator> Octree::getSurfaceVoxels(char connectivity, int adjacentVoxelBorderSize)
{
	std::set<Ogre::Vector3, VectorLessThanComparator> ret;

	//call the recursive getSurfaceVoxels() method on the root
	ret = root->getSurfaceVoxels(Ogre::Vector3(0.0), this->size, this, connectivity, adjacentVoxelBorderSize);

	return ret;
}


bool Octree::isEdgeVoxel(Ogre::Vector3 pos, char connectivity)
{
	//if we've got an empty voxel
	if (this->at(pos.x, pos.y, pos.z).solid == SPACE_EMPTY)
	{
		return false;
	}
	//if it's solid, then check if it borders and empty space.
	else
	{
		if (connectivity == 6)
		{
			return ((this->at(pos.x, pos.y, pos.z-1).solid == SPACE_EMPTY) ||
				(this->at(pos.x, pos.y, pos.z+1).solid == SPACE_EMPTY) ||
				(this->at(pos.x, pos.y-1, pos.z).solid == SPACE_EMPTY) ||
				(this->at(pos.x, pos.y+1, pos.z).solid == SPACE_EMPTY) ||
				(this->at(pos.x-1, pos.y, pos.z).solid == SPACE_EMPTY) ||
				(this->at(pos.x+1, pos.y, pos.z).solid == SPACE_EMPTY) );
		}
		else if (connectivity == 18)
		{
			return ((this->at(pos.x, pos.y, pos.z-1).solid == SPACE_EMPTY) ||
				(this->at(pos.x, pos.y, pos.z+1).solid == SPACE_EMPTY) ||
				(this->at(pos.x, pos.y-1, pos.z-1).solid == SPACE_EMPTY) ||
				(this->at(pos.x, pos.y-1, pos.z+1).solid == SPACE_EMPTY) ||
				(this->at(pos.x, pos.y+1, pos.z-1).solid == SPACE_EMPTY) ||
				(this->at(pos.x, pos.y+1, pos.z+1).solid == SPACE_EMPTY) ||
				
				(this->at(pos.x, pos.y-1, pos.z).solid == SPACE_EMPTY) ||
				(this->at(pos.x, pos.y+1, pos.z).solid == SPACE_EMPTY) ||
				(this->at(pos.x-1, pos.y-1, pos.z).solid == SPACE_EMPTY) ||
				(this->at(pos.x-1, pos.y+1, pos.z).solid == SPACE_EMPTY) ||
				(this->at(pos.x+1, pos.y-1, pos.z).solid == SPACE_EMPTY) ||
				(this->at(pos.x+1, pos.y+1, pos.z).solid == SPACE_EMPTY) ||
				
				(this->at(pos.x-1, pos.y, pos.z).solid == SPACE_EMPTY) ||
				(this->at(pos.x+1, pos.y, pos.z).solid == SPACE_EMPTY) ||
				(this->at(pos.x-1, pos.y, pos.z-1).solid == SPACE_EMPTY) ||
				(this->at(pos.x+1, pos.y, pos.z-1).solid == SPACE_EMPTY) ||
				(this->at(pos.x-1, pos.y, pos.z+1).solid == SPACE_EMPTY) ||
				(this->at(pos.x+1, pos.y, pos.z+1).solid == SPACE_EMPTY) );
		}
		else
		{
			//default case is 26

			//the 18 case...
			return ((this->at(pos.x, pos.y, pos.z-1).solid == SPACE_EMPTY) ||
				(this->at(pos.x, pos.y, pos.z+1).solid == SPACE_EMPTY) ||
				(this->at(pos.x, pos.y-1, pos.z-1).solid == SPACE_EMPTY) ||
				(this->at(pos.x, pos.y-1, pos.z+1).solid == SPACE_EMPTY) ||
				(this->at(pos.x, pos.y+1, pos.z-1).solid == SPACE_EMPTY) ||
				(this->at(pos.x, pos.y+1, pos.z+1).solid == SPACE_EMPTY) ||
				
				(this->at(pos.x, pos.y-1, pos.z).solid == SPACE_EMPTY) ||
				(this->at(pos.x, pos.y+1, pos.z).solid == SPACE_EMPTY) ||
				(this->at(pos.x-1, pos.y-1, pos.z).solid == SPACE_EMPTY) ||
				(this->at(pos.x-1, pos.y+1, pos.z).solid == SPACE_EMPTY) ||
				(this->at(pos.x+1, pos.y-1, pos.z).solid == SPACE_EMPTY) ||
				(this->at(pos.x+1, pos.y+1, pos.z).solid == SPACE_EMPTY) ||
				
				(this->at(pos.x-1, pos.y, pos.z).solid == SPACE_EMPTY) ||
				(this->at(pos.x+1, pos.y, pos.z).solid == SPACE_EMPTY) ||
				(this->at(pos.x-1, pos.y, pos.z-1).solid == SPACE_EMPTY) ||
				(this->at(pos.x+1, pos.y, pos.z-1).solid == SPACE_EMPTY) ||
				(this->at(pos.x-1, pos.y, pos.z+1).solid == SPACE_EMPTY) ||
				(this->at(pos.x+1, pos.y, pos.z+1).solid == SPACE_EMPTY) ||

			//and the other 8

				(this->at(pos.x-1, pos.y-1, pos.z-1).solid == SPACE_EMPTY) ||
				(this->at(pos.x-1, pos.y-1, pos.z+1).solid == SPACE_EMPTY) ||
				(this->at(pos.x-1, pos.y+1, pos.z-1).solid == SPACE_EMPTY) ||
				(this->at(pos.x-1, pos.y+1, pos.z+1).solid == SPACE_EMPTY) ||
				(this->at(pos.x+1, pos.y-1, pos.z-1).solid == SPACE_EMPTY) ||
				(this->at(pos.x+1, pos.y-1, pos.z+1).solid == SPACE_EMPTY) ||
				(this->at(pos.x+1, pos.y+1, pos.z-1).solid == SPACE_EMPTY) ||
				(this->at(pos.x+1, pos.y+1, pos.z+1).solid == SPACE_EMPTY) );
		}
	}
}


//converts a set of strings into a python list
PyObject* Octree::convertToList(const std::set<std::string>& in)
{	
        //the list object we're adding stuff to.
	PyObject* ret = PyList_New(0);

	//used for storing return status codes of the adding methods.
	int status;
	PyObject* temp;

	//go through all the strings in the set and add them to the list
	for (std::set<std::string>::iterator i = in.begin(); i != in.end(); i++)
	{
		temp = PyString_FromString(i->c_str());
		status = PyList_Append(ret, temp);
		Py_DECREF(temp);

		//if an error occurs output a warning message
		if (status != 0)
		{
			std::cout << "ERROR: An error occured during list appending." << "\n";
		}
	}

	return ret;
}


//convert a Vector3 into a list representation.
PyObject* Octree::convertToList(const Ogre::Vector3& in)
{
        //the list object we're adding stuff to.
	PyObject* ret = PyList_New(0);

	//used for storing return status codes of the adding methods.
	int status;
	PyObject* temp;

	//add the three components to the list
	temp = PyFloat_FromDouble(in.x);
	status = PyList_Append(ret, temp);
	Py_DECREF(temp);

	temp = PyFloat_FromDouble(in.y);
	status = PyList_Append(ret, temp);
	Py_DECREF(temp);

	temp = PyFloat_FromDouble(in.z);
	status = PyList_Append(ret, temp);
	Py_DECREF(temp);
	
	return ret;	
}


//this method takes a VoxelInformation object and returns a python list representation of it.
PyObject* Octree::convertToDict(const VoxelInformation& in)
{
	//the list object we're adding stuff to.
	PyObject* ret = PyDict_New();

	//used for storing return status codes of the adding methods.
	int status;
	PyObject* temp_list, *name;

	//now go through each of the things in the VoxelInformation and add them to the list.

	name = PyString_FromString("solid");
	status = PyDict_SetItem(ret, name, PyBool_FromLong(in.solid));
	Py_DECREF(name);

	name = PyString_FromString("tags");
	temp_list = convertToList(in.tags);
	status = PyDict_SetItem(ret, name, temp_list);
	Py_DECREF(temp_list);
	Py_DECREF(name);

	name = PyString_FromString("aggregate_solid");
	status = PyDict_SetItem(ret, name, PyBool_FromLong(in.aggregate_solid));
	Py_DECREF(name);

	name = PyString_FromString("aggregate_tags");
	temp_list = convertToList(in.aggregate_tags);
	status = PyDict_SetItem(ret, name, temp_list);
	Py_DECREF(temp_list);
	Py_DECREF(name);

	name = PyString_FromString("aggregate_normal");
	temp_list = convertToList(in.aggregate_normal);
	status = PyDict_SetItem(ret, name, temp_list);
	Py_DECREF(temp_list);
	Py_DECREF(name);

	name = PyString_FromString("detail_info");
	temp_list = convertToList(in.detail_info);
	status = PyDict_SetItem(ret, name, temp_list);
	Py_DECREF(temp_list);
	Py_DECREF(name);

	return ret;
}


//this method reads in the python file in the argument and runs the
//rule set contained in it on the Octree.
//The rules_method argument is the name of the method in the file to call on each voxel.
void Octree::runAutomataRules(std::string rules_file, std::string rules_method, int max_iterations, int neighbourhood_radius)
{
	//initialize the python interpreter
	Py_Initialize();

	//make the python objects that we need to run the grammar python code.
	PyObject *p_rule_set_module, *p_error_name, *p_rule_set_dictionary, *p_rule_set_function;

	//this stores the integer return values (status/error codes) from the Py*_ methods
	int status;

	//append the contents folder to the system path so python looks for modules there.
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append(\"./content/\")");

	//initialize the random number generator
	PyRun_SimpleString("import random");
	PyRun_SimpleString("random.seed()");

	//load the file containing the rule set.
	p_rule_set_module = PyImport_ImportModule(rules_file.c_str());

	//check if there was an error importing the file with the rules.
	p_error_name = PyErr_Occurred();
	if (p_error_name != NULL)
	{
		std::cout << "========================================" << "\n";
		PyErr_Print();
		std::cout << "========================================" << "\n";
	}

	//get the rule file's dictionary
	p_rule_set_dictionary = PyModule_GetDict(p_rule_set_module);
	
	//now get the number of iterations of the CA to do from the module, if it's not given as an argument.
	//default value of 1
	int num_iterations = 1;
	//if the argument value is 1 or more (i.e. valid)
	if (max_iterations >= 1)
	{
		//then use that
		num_iterations = max_iterations;
	}
	//otherwise, we need to get the value from the file.
	else
	{
		
		p_rule_set_function = PyDict_GetItemString(p_rule_set_dictionary, "num_iterations");
		if (p_rule_set_function == NULL)
		{
			std::cout << "WARNING: could not get num_iterations from rule set. Using default value." << "\n";
		}
		else
		{
			num_iterations = PyInt_AsLong(p_rule_set_function);
		}	
	}

        //This stores the size of the neighbourhood around the voxel that we'll examine for the automata rules.
	int neighbourhood_size = 0;
	//if the neighbourhood size argument is 0 or more (i.e. valid)
	if (neighbourhood_radius >= 0)
	{
		//then use the argument
		neighbourhood_size = neighbourhood_radius;
	}
	//otherwise
	else
	{
		//we get the value to use from the python rule set.
		p_rule_set_function = PyDict_GetItemString(p_rule_set_dictionary, "neighbourhood_size");
		if (p_rule_set_function == NULL)
		{
			std::cout << "WARNING: could not get neighbourhood_size from rule set. Using default value." << "\n";
		}
		else
		{
			neighbourhood_size = PyInt_AsLong(p_rule_set_function);	
		}	
	}

	//if the rules method is the empty string, then default to looking for a method named 'main'
	if (rules_method == "")
	{
		p_rule_set_function = PyDict_GetItemString(p_rule_set_dictionary, "main");	
	}
	//otherwise use whatever rules_method is.
	else
	{
		p_rule_set_function = PyDict_GetItemString(p_rule_set_dictionary, rules_method.c_str());
	}
	
	if (!PyCallable_Check(p_rule_set_function))
	{
		std::cout << "ERROR: no function named " << rules_method << " found in the rule set file " << rules_file << "\n";
		PyErr_Print();
		std::cout << "no detailing will be done." << "\n";
		Py_Finalize();
		return;
	}
	
	//check if an error occured
	p_error_name = PyErr_Occurred();
	if (p_error_name != NULL)
	{
		std::cout << "========================================" << "\n";
		PyErr_Print();
		std::cout << "========================================" << "\n";
	}

	//make a copy of the tree that we can use as a back buffer
	//when running multiple iterations of the automata.
	Octree* back_automata_buffer = new Octree(*this);
	//these pointers are used for switching between the front and
	//back buffer for doing multiple iterations of the
	//automata. Data is read from one and written to the other in
	//each iteration, and between iterations, which is read and
	//which is written to is swapped.
	Octree *current_buffer = NULL, *new_buffer = NULL;

	//to make sure that we end up with *this (the Octree this
	//method was called on) having the final state of the cellular
	//automata after we're done running the iterations, we need to
	//assign it either current or new, depending on whether the
	//number of iterations is odd or even.
	if (num_iterations % 2 == 0)
	{
		new_buffer = back_automata_buffer;
		current_buffer = this;
	}
	else
	{
		new_buffer = this;
		current_buffer = back_automata_buffer;
	}

	//get the surface voxels from the octree, so we only operate on those.
	std::set<Ogre::Vector3, VectorLessThanComparator> surface_voxels = this->getSurfaceVoxels(26, 0);
	//temp variables
	VoxelInformation curr_voxel;
	
	//now iterate the number of times specified in the file
	for (int iteration_count = 1; iteration_count <= num_iterations; ++iteration_count)
	{
		std::cout << "surface detail: iteration: " << iteration_count << "\n"; //TEMP
		int my_count = 0;
		
		//loop over every surface voxel
		for (std::set<Ogre::Vector3, VectorLessThanComparator>::iterator i = surface_voxels.begin(); i != surface_voxels.end(); i++)
		{
			my_count++;
			if (my_count % 1000 == 0)
			{
				std::cout << "doing voxel " << my_count << " of " << surface_voxels.size() << "\n";
			}
			
			//get the current voxel's information.
			curr_voxel = current_buffer->at(i->x, i->y, i->z);
			//put it's neighbours into a python array.

			//this will store the neighbourhood around the
			//voxel of interest, and get given to the
			//python grammar rule method.
			PyObject* neighbours = PyList_New(0);

			//temporary variables
			PyObject *y_row = NULL, *z_row = NULL;
			PyObject* temp_list = NULL;

			//loop over the entire neighbourhood and add each thing to the array.
			for (int x = -neighbourhood_size; x <= neighbourhood_size; ++x)
			{
				y_row = PyList_New(0);
				
				for (int y = -neighbourhood_size; y <= neighbourhood_size; ++y)
				{
					z_row = PyList_New(0);
					
					for (int z = -neighbourhood_size; z <= neighbourhood_size; ++z)
					{	
						temp_list = convertToDict(current_buffer->at(i->x + x, i->y + y, i->z + z));
						
						status = PyList_Append(z_row, temp_list);
						if (status != 0)
						{
							std::cout << "ERROR: could not insert list element." << "\n"; //TEMP 
						}
					}

					status = PyList_Append(y_row, z_row);
					if (status == -1)
					{
						std::cout << "ERROR: could not insert list element." << "\n"; //TEMP 
					}
				}

				status = PyList_Append(neighbours, y_row);
				if (status == -1)
				{
					std::cout << "ERROR: could not insert list element." << "\n"; //TEMP 
				}
			}
						
			//this is the python list containing the current voxel that gets passed to the python method. It's already contained in the above array, but this is for convenience.
			PyObject* p_curr_voxel = convertToDict(curr_voxel);

			PyObject* p_curr_pos = convertToList(*i);
			PyObject* p_max_octree_size = convertToList(Ogre::Vector3(this->size));
			PyObject* p_curr_iteration = PyInt_FromLong(iteration_count);

			//create the arguments for the python method from the current voxel, and it's neighbours' information
			PyObject* method_args = PyTuple_New(5);
			PyTuple_SetItem(method_args, 0, p_curr_voxel);
			PyTuple_SetItem(method_args, 1, neighbours);
			PyTuple_SetItem(method_args, 2, p_curr_pos);
			PyTuple_SetItem(method_args, 3, p_max_octree_size);
			PyTuple_SetItem(method_args, 4, p_curr_iteration);

			//run the python rule on it.
			PyObject* returned_object = PyObject_CallObject(p_rule_set_function, method_args);

			if (returned_object == NULL)
			{
				std::cout << "ERROR: could not successfully call python method" << "\n";
				std::cout << "================================================\n";
				PyErr_Print();
				std::cout << "================================================\n";
				std::cout << "       skipping on to next voxel." << "\n\n";
				exit(1);
				//continue;
			}
			
			//assign the rule output to curr_voxel
			//format of returned object is list containing new detail_info tags.
			if (!PyList_Check(returned_object))
			{
				std::cout << "ERROR: object returned from rule set is not a list." << "\n";
				std::cout << "       Current voxel will not be updated." << "\n";
			}
			else
			{
				int list_size = PyList_Size(returned_object);
				curr_voxel.detail_info.clear();
				for (int j = 0; j < list_size; ++j)
				{
					curr_voxel.detail_info.insert( std::string(PyString_AsString( PyList_GetItem(returned_object, j)) ) );
				}
			}

			//then assign the rule's output to the new_buffer
			new_buffer->set(i->x, i->y, i->z, curr_voxel);

			//DECREF the unneeded objects.
			
			//this block of code DECREF's all the contents of the neighbourhood array
			for (int x = 0; x < PyList_Size(neighbours); ++x)
			{
				PyObject* x_list = PyList_GetItem(neighbours, x);
								
				for (int y = 0; y < PyList_Size(x_list); ++y)
				{
					PyObject* y_list = PyList_GetItem(x_list, y);
					
					for (int z = 0; z < PyList_Size(y_list); ++z)
					{
						PyDict_Clear(PyList_GetItem(y_list, z));
						Py_DECREF(PyList_GetItem(y_list, z));
					}

					Py_DECREF(y_list);
				}

				Py_DECREF(x_list);
			}
			Py_DECREF(temp_list);
			Py_DECREF(y_row);
			Py_DECREF(z_row);

			Py_DECREF(p_curr_pos);
			Py_DECREF(p_max_octree_size);

			Py_DECREF(neighbours);
			Py_DECREF(p_curr_voxel);
			Py_DECREF(p_curr_voxel);
			Py_DECREF(returned_object);
			Py_DECREF(method_args);
		}

		//swap the buffer pointers.
		Octree* temp = new_buffer;
		new_buffer = current_buffer;
		current_buffer = temp;
	}

	delete back_automata_buffer;
	
	//dereference the python objects we created
	Py_DECREF(p_rule_set_module);
	Py_DECREF(p_rule_set_dictionary);
	Py_DECREF(p_rule_set_function);
	
	//shut down the python interpreter
	Py_Finalize();
}


//This method returns a set containing the positions of all solid voxels in the grid.
std::set<Ogre::Vector3, VectorLessThanComparator> Octree::getAllSolidVoxels()
{
	std::set<Ogre::Vector3, VectorLessThanComparator> ret;

	//call the recursive getSurfaceVoxels() method on the root
	ret = root->getSurfaceVoxels(Ogre::Vector3(0.0), this->size, this);

	return ret;	
}


//====================OctreeNode method implementations=========================


//default constructor.
OctreeNode::OctreeNode()
{
	VoxelInformation temp;
	this->createNode(1, temp);
}


//copy constructor.
OctreeNode::OctreeNode(const OctreeNode& copy_from_me)
{
	this->info = copy_from_me.info;
	this->nodeSize = copy_from_me.nodeSize;

	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			for (int k = 0; k < 2; ++k)
			{
				if (copy_from_me.children[i][j][k] == NULL)
				{
					this->children[i][j][k] = NULL;
				}
				else
				{
					this->children[i][j][k] = new OctreeNode(*(copy_from_me.children[i][j][k]));
				}
			}
}


//recursive destructor
OctreeNode::~OctreeNode()
{
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			for (int k = 0; k < 2; ++k)
			{
				if (this->children[i][j][k] != NULL)
				{
					delete this->children[i][j][k];
					this->children[i][j][k] = NULL;
				}
			}
}


OctreeNode::OctreeNode(int in_maxSize)
{
	VoxelInformation temp;
	this->createNode(in_maxSize, temp);
}


OctreeNode::OctreeNode(int in_maxSize, VoxelInformation in_info)
{
	this->createNode(in_maxSize, in_info);
}


//this is the method that's actually used by the constructors to make
//the node.
void OctreeNode::createNode(int in_maxSize, VoxelInformation in_info)
{
	this->nodeSize = in_maxSize;
	this->info = in_info;
	
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			for (int k = 0; k < 2; ++k)
			{
				this->children[i][j][k] = NULL;
			}
}


//get and set methods for nodeSize. can probably ignore these methods
//(just use ->nodeSize and ->nodeSize =) for performance gains.
void OctreeNode::setNodeSize(int in)
{
	//check the size is a valid value, both positive and a power of 2
	assert(in > 0);
	assert( ((in - 1) & in) == 0 );

	this->nodeSize = in;
}


int OctreeNode::getNodeSize()
{
	return this->nodeSize;
}


//Recursive method for retrieving a value from the octree.  currSize
//is used for choosing the node to recurse to. depth_restriction is a
//way of constraining the depth in the tree that this method can
//recurse to. It's the size of the nodes that the method should stop
//recursing at.
VoxelInformation OctreeNode::at(int x, int y, int z, int currSize, int depth_restriction)
{
	//if we've hit our depth_restriction, then return the current
	//node's info, and do not recurse any further.
	if ((depth_restriction > 0) && (currSize <= depth_restriction))
	{
		return this->info;
	}
	
	//if we're at a node with no children
	if ((this->children[0][0][0] == NULL) &&
	    (this->children[0][0][1] == NULL) &&
	    (this->children[0][1][0] == NULL) &&
	    (this->children[0][1][1] == NULL) &&
	    (this->children[1][0][0] == NULL) &&
	    (this->children[1][0][1] == NULL) &&
	    (this->children[1][1][0] == NULL) &&
	    (this->children[1][1][1] == NULL))
	{
		//return the current node's VoxelInformation
		return this->info;
	}
	//otherwise if there are children
	else
	{
		//I got this code for selecting which child to go to from the octree class I was using. hope it works.
		int newSize = currSize / 2;
		//recurse on the correct child node if it's non-null, else return the current node's value
		if (this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)] == NULL)
		{
			return this->info;
		}
		else
		{
			return this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)]->at(x,y,z, newSize, depth_restriction);
		}		
	}
}


//as above, but returns a pointer instead of a copy of the voxel info
VoxelInformation* OctreeNode::at_pointer(int x, int y, int z, int currSize, int depth_restriction)
{
	//if we've hit our depth_restriction, then return the current
	//node's info, and do not recurse any further.
	if ((depth_restriction > 0) && (currSize <= depth_restriction))
	{
		return &(this->info);
	}
	
	//if we're at a node with no children
	if ((this->children[0][0][0] == NULL) &&
	    (this->children[0][0][1] == NULL) &&
	    (this->children[0][1][0] == NULL) &&
	    (this->children[0][1][1] == NULL) &&
	    (this->children[1][0][0] == NULL) &&
	    (this->children[1][0][1] == NULL) &&
	    (this->children[1][1][0] == NULL) &&
	    (this->children[1][1][1] == NULL))
	{
		//return the current node's VoxelInformation
		return &(this->info);
	}
	//otherwise if there are children
	else
	{
		//I got this code for selecting which child to go to from the octree class I was using. hope it works.
		int newSize = currSize / 2;
		//recurse on the correct child node if it's non-null, else return the current node's value
		if (this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)] == NULL)
		{
			return &(this->info);
		}
		else
		{
			return this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)]->at_pointer(x,y,z, newSize, depth_restriction);
		}		
	}
}


//A recursive method for setting a value. It avoids creating
//unnecessary nodes, where possible.
void OctreeNode::set(int x, int y, int z, VoxelInformation value, int currSize)
{
	//if we're at a leaf node
	if (currSize == 1)
	{
		//then set it to the value
		this->info = value;
	}
	//if the current node has no children and is already set to the given value
	else if ((this->info == value) &&
		 (this->children[0][0][0] == NULL) &&
		 (this->children[0][0][1] == NULL) &&
		 (this->children[0][1][0] == NULL) &&
		 (this->children[0][1][1] == NULL) &&
		 (this->children[1][0][0] == NULL) &&
		 (this->children[1][0][1] == NULL) &&
		 (this->children[1][1][0] == NULL) &&
		 (this->children[1][1][1] == NULL) )
	{
		//then we don't need to do anything, since the value is already set.
		return;
	}
	//otherwise, we need to recurse
	else
	{
		//determine which child the element to set lies in.
		int newSize = currSize / 2;
		OctreeNode* nextNode = this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)];

		//if that child node is currently empty.
		if (nextNode == NULL)
		{
			//then instantiate it.
			this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)] = new OctreeNode(newSize, this->info);
			this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)]->info = this->info;
			this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)]->set(x,y,z, value, newSize);
		}
		else
		{
			//otherwise, recurse on it.
			nextNode->set(x,y,z, value, newSize);
		}
	}
}


//recursive method for setting an axis-aligned rectangle of values in the octree.
void OctreeNode::setRange(Ogre::Vector3 lower, Ogre::Vector3 upper, Ogre::Vector3 node_center, VoxelInformation value)
{
	
}


//A recursive method for deleting a node. This means removing it from
//the tree, not just changing it's VoxelInformation to empty.
void OctreeNode::erase(int x, int y, int z, int currSize)
{
        //if we're at a node with no children
	if ((this->children[0][0][0] == NULL) &&
	    (this->children[0][0][1] == NULL) &&
	    (this->children[0][1][0] == NULL) &&
	    (this->children[0][1][1] == NULL) &&
	    (this->children[1][0][0] == NULL) &&
	    (this->children[1][0][1] == NULL) &&
	    (this->children[1][1][0] == NULL) &&
	    (this->children[1][1][1] == NULL))
	{
		//then the value does not need to be erased.
		return;
	}
	else
	{
		//determine which child the element to erase lies in.
		int newSize = currSize / 2;
		OctreeNode* nextNode = this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)];

		//if the child to delete is a leaf node
		if (newSize == 1)
		{
			//then delete it, if it isn't already NULL
			if (nextNode != NULL)
			{
				delete this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)];
				this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)] = NULL;
			}
		}
		//otherwise, recurse on it
		else
		{
			this->children[!!(x & newSize)][!!(y & newSize)][!!(z & newSize)]->erase(x,y,z, newSize);
		}
	}
}


//recursive method for erasing an axis-aligned rectangle of nodes from the tree.
void OctreeNode::eraseRange(Ogre::Vector3 lower, Ogre::Vector3 upper, Ogre::Vector3 node_center)
{
	
}


//This method does a postorder walk of the tree, removing redundant
//nodes. If all children of a node are non-NULL and they all have the
//same VoxelInformation, they can be deleted and the parent's
//VoxelInformation changed to theirs.
void OctreeNode::optimizeNode()
{
	//first recurse on all children.
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			for (int k = 0; k < 2; ++k)
			{
				if (this->children[i][j][k] != NULL)
					this->children[i][j][k]->optimizeNode();
			}
	
        //if we're at a node with no children
	if ((this->children[0][0][0] == NULL) &&
	    (this->children[0][0][1] == NULL) &&
	    (this->children[0][1][0] == NULL) &&
	    (this->children[0][1][1] == NULL) &&
	    (this->children[1][0][0] == NULL) &&
	    (this->children[1][0][1] == NULL) &&
	    (this->children[1][1][0] == NULL) &&
	    (this->children[1][1][1] == NULL))
	{
		//then we can't actually do anything with this node.
		return;
	}
	//if the node has children
	else
	{
		//if all the children are non-NULL
		if ((this->children[0][0][0] != NULL) &&
		    (this->children[0][0][1] != NULL) &&
		    (this->children[0][1][0] != NULL) &&
		    (this->children[0][1][1] != NULL) &&
		    (this->children[1][0][0] != NULL) &&
		    (this->children[1][0][1] != NULL) &&
		    (this->children[1][1][0] != NULL) &&
		    (this->children[1][1][1] != NULL))
		{
			//then check if they all have the same info.
			//NB: we know that all children are non-NULL
		       
			//comp_value stores info of the first child. if any of the other children have a different ->info, we can't coalesce them all into the parent, so we check if any are different to comp_value
			VoxelInformation comp_value = this->children[0][0][0]->info;
			
			bool can_coalesce = true;

			//loop through all children
			for (int i = 0; i < 2; ++i)
				for (int j = 0; j < 2; ++j)
					for (int k = 0; k < 2; ++k)
					{
						//if any of the children of the child currently being examined are non-NULL, we can't optimize away this node.
						if ((this->children[i][j][k]->children[0][0][0] != NULL) ||
						    (this->children[i][j][k]->children[0][0][1] != NULL) ||
						    (this->children[i][j][k]->children[0][1][0] != NULL) ||
						    (this->children[i][j][k]->children[0][1][1] != NULL) ||
						    (this->children[i][j][k]->children[1][0][0] != NULL) ||
						    (this->children[i][j][k]->children[1][0][1] != NULL) ||
						    (this->children[i][j][k]->children[1][1][0] != NULL) ||
						    (this->children[i][j][k]->children[1][1][1] != NULL))
						{
							//return, since this node can't be optimized.
							can_coalesce = false;
							break;
						}
						//if they don't all have the same info
						if (this->children[i][j][k]->info != comp_value)
						{
							//return, since this node can't be optimized.
							can_coalesce = false;
							break;
						}
					}

			//if the children of the node can be optimized away
			if (can_coalesce)
			{
				//then set this node's value to the children's
				this->info = comp_value;

				//and remove the children.
				for (int i = 0; i < 2; ++i)
					for (int j = 0; j < 2; ++j)
						for (int k = 0; k < 2; ++k)
						{
							delete this->children[i][j][k];
							this->children[i][j][k] = NULL;
						}
			}
		}
		//if the children are a mix of NULL and non-NULL
		else
		{
			//loop through all the children.
			for (int i = 0; i < 2; ++i)
				for (int j = 0; j < 2; ++j)
					for (int k = 0; k < 2; ++k)
					{
						if (this->children[i][j][k] != NULL)
						{
							//this checks if all children of the child being
							//examined are null. If they are, then the current
							//child can be made NULL.
							bool all_children_null = true;
							for (int x = 0; x < 2; ++x)
								for (int y = 0; y < 2; ++y)
									for (int z = 0; z < 2; ++z)
									{
										if (this->children[i][j][k]->children[x][y][z] != NULL)
										{
											all_children_null = false;
										}
									}
							if ((all_children_null) && (this->children[i][j][k]->info.solid == this->info.solid))
							{
								delete this->children[i][j][k];
								this->children[i][j][k] = NULL;
							}	
						}
						
					}
		}
	}
}


//this method returns a string representation of the current node and
//it's children. depth is used in formatting the string correctly.
std::string OctreeNode::printNode(int depth)
{
	std::string ret = "";

	//add in the correct number of spaces
	for (int i = 0; i < depth; ++i)
	{
		ret += "  ";
	}

	//append this node's information
	ret += "Node: ";
	ret += Utility::numToString(this->nodeSize);
	ret += " ";
	ret += Utility::numToString(this->info.solid);
	ret += " | Tags: ";
	for (std::set<std::string>::iterator i = this->info.tags.begin(); i != this->info.tags.end(); i++)
	{
		ret = ret + *i + " ";
	}
	ret += " | Aggregate solid: ";
	ret += Utility::numToString(this->info.aggregate_solid);
	ret += " | Aggregate normal: ";
	ret += Utility::ogreVector3ToString(this->info.aggregate_normal);
	ret += " | Aggregate Tags: ";
	for (std::set<std::string>::iterator i = this->info.aggregate_tags.begin(); i != this->info.aggregate_tags.end(); i++)
	{
		ret = ret + *i + " ";
	}
	ret += " | Detailing Info: ";
	for (std::set<std::string>::iterator i = this->info.detail_info.begin(); i != this->info.detail_info.end(); i++)
	{
		ret = ret + *i + " ";
	}
	ret += "\n";

	//if this is not a leaf node
	if (this->nodeSize > 1)
	{
		//then recurse on any non-NULL children and add their string representation to the return string.
		for (int i = 0; i < 2; ++i)
			for (int j = 0; j < 2; ++j)
				for (int k = 0; k < 2; ++k)
				{
					if (this->children[i][j][k] == NULL)
					{
						for (int i = 0; i < depth+1; ++i)
						{
							ret += "  ";
						}
						ret += "NULL\n";
					}
					else
					{
						ret += this->children[i][j][k]->printNode(depth + 1);
					}
				}	
	}

	return ret;
}


void OctreeNode::makeAggregateInformation()
{
        //first recurse on all children.
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			for (int k = 0; k < 2; ++k)
			{
				if (this->children[i][j][k] != NULL)
					this->children[i][j][k]->makeAggregateInformation();
			}

	//copy any tags the node may have into it's aggregate_tags field.
	this->info.aggregate_tags = std::set<std::string>(this->info.tags);
	
	//then get all the tags from the children and include them in this Node
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			for (int k = 0; k < 2; ++k)
			{
				if (this->children[i][j][k] != NULL)
				{
					for (std::set<std::string>::iterator a = this->children[i][j][k]->info.aggregate_tags.begin(); a != this->children[i][j][k]->info.aggregate_tags.end(); a++)
					{
						this->info.aggregate_tags.insert(*a);
					}
				}
			}

	//then decide whether this aggregate node is solid or not.

	//if all the children are NULL, then just assign the current node's solidness as it's aggregate solidness
	if ((this->children[0][0][0] == NULL) &&
	    (this->children[0][0][1] == NULL) &&
	    (this->children[0][1][0] == NULL) &&
	    (this->children[0][1][1] == NULL) &&
	    (this->children[1][0][0] == NULL) &&
	    (this->children[1][0][1] == NULL) &&
	    (this->children[1][1][0] == NULL) &&
	    (this->children[1][1][1] == NULL))
	{
		this->info.aggregate_solid = this->info.solid;
	}
	else
	{	
		//this is currently a *very* simplistic way of doing it, I should probably do a better one later.
		//this int stores how many of the child nodes have a different solidness from the current node.
		int solid_children_count = 0;
		for (int i = 0; i < 2; ++i)
			for (int j = 0; j < 2; ++j)
				for (int k = 0; k < 2; ++k)
				{
					if (this->children[i][j][k] != NULL)
					{
						if (this->children[i][j][k]->info.aggregate_solid == SPACE_SOLID)
						{
							solid_children_count++;
						}
					}
					else
					{
						if (this->info.solid == SPACE_SOLID)
						{
							solid_children_count++;
						}
					}
				}

		//if even one child is solid, the parent is solid (to
		//ensure full coverage for the cellular automata.
		if (solid_children_count > 0)
		{
			this->info.aggregate_solid = SPACE_SOLID;
		}
		else
		{
			this->info.aggregate_solid = SPACE_EMPTY;
		}
	}

	//if this is a solid node, then calculate it's aggregate normal.
	if ((this->nodeSize > 1) && (this->info.aggregate_solid == SPACE_SOLID))
	{
		//average the child normals to get this node's normal
		Ogre::Vector3 total = Ogre::Vector3(0.0);
		int count = 0;
		
		for (int i = 0; i < 2; ++i)
			for (int j = 0; j < 2; ++j)
				for (int k = 0; k < 2; ++k)
				{
					if (this->children[i][j][k] != NULL)
					{
						count++;
						total += this->children[i][j][k]->info.aggregate_normal;
					}
				}

		if (count == 0)
		{
			this->info.aggregate_normal = Ogre::Vector3::ZERO;
		}
		else
		{
			this->info.aggregate_normal = total / count;
			this->info.aggregate_normal.normalise();	
		}

		//alternatively, we can get a normal by comparing to aggregated neighbours at this resolution.
		
		//this is commented out because this method currently
		//has no way of knowing a node's position. If there
		//are problems with the averaging method, then add
		//positional information to this method and adjust the
		//code below to use it.
		
		// temp.aggregate_normal.x = grid->at(i->x - 1, i->y, i->z).solid - grid->at(i->x + 1, i->y, i->z).solid;
		// temp.aggregate_normal.y = grid->at(i->x, i->y - 1, i->z).solid - grid->at(i->x, i->y + 1, i->z).solid;
		// temp.aggregate_normal.z = grid->at(i->x, i->y, i->z - 1).solid - grid->at(i->x, i->y, i->z + 1).solid;
		// temp.aggregate_normal.normalise();
	}
}


//This method returns a vector containing all the surface voxels in
//the current octree node.  adjacentVoxelBorderSize is the size of the
//region around a surface voxel that should be returned to. It
//defaults to 0, but to do marchin cubes correctly, neighbouring
//voxels must be marched as well, which is why this option is included.
std::set<Ogre::Vector3, VectorLessThanComparator> OctreeNode::getSurfaceVoxels(Ogre::Vector3 corner, int currSize, Octree* tree, char connectivity, int adjacentVoxelBorderSize)
{
	//this contains the return results, starts off empty.
	std::set<Ogre::Vector3, VectorLessThanComparator> ret;
	
	//then check each of it's surface elements for edginess.
	Ogre::Vector3 curr_pos;

	if (this->info.solid == SPACE_SOLID)
	{	
		for (int a = 0; a < (currSize); ++a)
			for (int b = 0; b < (currSize); ++b)
			{
				curr_pos = corner + Ogre::Vector3(0,a,b);
				if (tree->isEdgeVoxel(curr_pos, connectivity))
				{
					if (adjacentVoxelBorderSize > 0)
					{
						//add all the surrounding voxels to the return list too
						for (int i = -adjacentVoxelBorderSize; i <= adjacentVoxelBorderSize; ++i)
						{
							for (int j = -adjacentVoxelBorderSize; j <= adjacentVoxelBorderSize; ++j)
							{
								for (int k = -adjacentVoxelBorderSize; k <= adjacentVoxelBorderSize; ++k)
								{
									ret.insert(curr_pos + Ogre::Vector3(i,j,k));
								}
							}
						}	
					}
					else
					{
						ret.insert(curr_pos);
					}
				}
			}
							
		for (int a = 0; a < (currSize); ++a)
			for (int b = 0; b < (currSize); ++b)
			{
				curr_pos = corner + Ogre::Vector3(a,0,b);
				if (tree->isEdgeVoxel(curr_pos, connectivity))
				{
					if (adjacentVoxelBorderSize > 0)
					{
						//add all the surrounding voxels to the return list too
						for (int i = -adjacentVoxelBorderSize; i <= adjacentVoxelBorderSize; ++i)
						{
							for (int j = -adjacentVoxelBorderSize; j <= adjacentVoxelBorderSize; ++j)
							{
								for (int k = -adjacentVoxelBorderSize; k <= adjacentVoxelBorderSize; ++k)
								{
									ret.insert(curr_pos + Ogre::Vector3(i,j,k));
								}
							}
						}	
					}
					else
					{
						ret.insert(curr_pos);
					}
				}
			}
							
		for (int a = 0; a < (currSize); ++a)
			for (int b = 0; b < (currSize); ++b)
			{
				curr_pos = corner + Ogre::Vector3(a,b,0);
				if (tree->isEdgeVoxel(curr_pos, connectivity))
				{
					if (adjacentVoxelBorderSize > 0)
					{
						//add all the surrounding voxels to the return list too
						for (int i = -adjacentVoxelBorderSize; i <= adjacentVoxelBorderSize; ++i)
						{
							for (int j = -adjacentVoxelBorderSize; j <= adjacentVoxelBorderSize; ++j)
							{
								for (int k = -adjacentVoxelBorderSize; k <= adjacentVoxelBorderSize; ++k)
								{
									ret.insert(curr_pos + Ogre::Vector3(i,j,k));
								}
							}
						}	
					}
					else
					{
						ret.insert(curr_pos);
					}
				}
			}
							
		for (int a = 0; a < (currSize); ++a)
			for (int b = 0; b < (currSize); ++b)
			{
				curr_pos = corner + Ogre::Vector3(currSize,a,b);
				if (tree->isEdgeVoxel(curr_pos, connectivity))
				{
					if (adjacentVoxelBorderSize > 0)
					{
						//add all the surrounding voxels to the return list too
						for (int i = -adjacentVoxelBorderSize; i <= adjacentVoxelBorderSize; ++i)
						{
							for (int j = -adjacentVoxelBorderSize; j <= adjacentVoxelBorderSize; ++j)
							{
								for (int k = -adjacentVoxelBorderSize; k <= adjacentVoxelBorderSize; ++k)
								{
									ret.insert(curr_pos + Ogre::Vector3(i,j,k));
								}
							}
						}	
					}
					else
					{
						ret.insert(curr_pos);
					}
				}
			}
							
		for (int a = 0; a < (currSize); ++a)
			for (int b = 0; b < (currSize); ++b)
			{
				curr_pos = corner + Ogre::Vector3(a,currSize,b);
				if (tree->isEdgeVoxel(curr_pos, connectivity))
				{
					if (adjacentVoxelBorderSize > 0)
					{
						//add all the surrounding voxels to the return list too
						for (int i = -adjacentVoxelBorderSize; i <= adjacentVoxelBorderSize; ++i)
						{
							for (int j = -adjacentVoxelBorderSize; j <= adjacentVoxelBorderSize; ++j)
							{
								for (int k = -adjacentVoxelBorderSize; k <= adjacentVoxelBorderSize; ++k)
								{
									ret.insert(curr_pos + Ogre::Vector3(i,j,k));
								}
							}
						}	
					}
					else
					{
						ret.insert(curr_pos);
					}
				}
			}
							
		for (int a = 0; a < (currSize); ++a)
			for (int b = 0; b < (currSize); ++b)
			{
				curr_pos = corner + Ogre::Vector3(a,b,currSize);
				if (tree->isEdgeVoxel(curr_pos, connectivity))
				{
					if (adjacentVoxelBorderSize > 0)
					{
						//add all the surrounding voxels to the return list too
						for (int i = -adjacentVoxelBorderSize; i <= adjacentVoxelBorderSize; ++i)
						{
							for (int j = -adjacentVoxelBorderSize; j <= adjacentVoxelBorderSize; ++j)
							{
								for (int k = -adjacentVoxelBorderSize; k <= adjacentVoxelBorderSize; ++k)
								{
									ret.insert(curr_pos + Ogre::Vector3(i,j,k));
								}
							}
						}	
					}
					else
					{
						ret.insert(curr_pos);
					}
				}
			}
	}
	
        //loop over the children.
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			for (int k = 0; k < 2; ++k)
			{
				if (this->children[i][j][k] != NULL)
				{
					//get the (lower) corner of the current node
					Ogre::Vector3 child_corner = Ogre::Vector3(corner.x + (i * currSize / 2), corner.y + (j * currSize / 2), corner.z + (k * currSize / 2));
				
					//and recurse on them
					std::set<Ogre::Vector3, VectorLessThanComparator> results = this->children[i][j][k]->getSurfaceVoxels(child_corner, currSize / 2, tree, connectivity, adjacentVoxelBorderSize);
					ret.insert(results.begin(), results.end());	
				}
			}
		
	return ret;	
}


std::set<Ogre::Vector3, VectorLessThanComparator> OctreeNode::getAllSolidVoxels(Ogre::Vector3 corner, int currSize, Octree* tree)
{
	//this contains the return results, starts off empty.
	std::set<Ogre::Vector3, VectorLessThanComparator> ret;

	if (this->info.solid == SPACE_SOLID)
	{
		for (int i = 0; i < currSize; ++i)
		{
			for (int j = 0; j < currSize; ++j)
			{
				for (int k = 0; k < currSize; ++k)
				{
					ret.insert(corner + Ogre::Vector3(i,j,k));
				}
			}
		}
	}
		
        //loop over the children.
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			for (int k = 0; k < 2; ++k)
			{
				if (this->children[i][j][k] != NULL)
				{
					//get the (lower) corner of the current node
					Ogre::Vector3 child_corner = Ogre::Vector3(corner.x + (i * currSize / 2), corner.y + (j * currSize / 2), corner.z + (k * currSize / 2));
				
					//and recurse on them
					std::set<Ogre::Vector3, VectorLessThanComparator> results = this->children[i][j][k]->getSurfaceVoxels(child_corner, currSize / 2, tree);
					ret.insert(results.begin(), results.end());	
				}
			}
		
	return ret;	
}


//This method returns true if all the children of the current node are NULL
bool OctreeNode::allChildrenNull()
{
	return ((this->children[0][0][0] == NULL) &&
		(this->children[0][0][1] == NULL) &&
		(this->children[0][1][0] == NULL) &&
		(this->children[0][1][1] == NULL) &&
		(this->children[1][0][0] == NULL) &&
		(this->children[1][0][1] == NULL) &&
		(this->children[1][1][0] == NULL) &&
		(this->children[1][1][1] == NULL));
}


//=================VoxelInformation methods========================


VoxelInformation::VoxelInformation()
{
	solid = SPACE_EMPTY;
	tags = std::set<std::string>();
	aggregate_solid = SPACE_EMPTY;
	aggregate_tags = std::set<std::string>();
	aggregate_normal = Ogre::Vector3(0.0);
	detail_info = std::set<std::string>();
}


VoxelInformation::VoxelInformation(SPACE_TYPE in_solid, std::set<std::string> in_tags)
{
	solid = in_solid;
	tags = std::set<std::string>(in_tags);
}


VoxelInformation::~VoxelInformation()
{

}


bool VoxelInformation::operator==(VoxelInformation& in)
{
	return ( (this->solid == in.solid) && (this->tags == in.tags) && (this->detail_info == in.detail_info) );
}


bool VoxelInformation::operator!=(VoxelInformation& in)
{
	return !( (this->solid == in.solid) && (this->tags == in.tags) && (this->detail_info == in.detail_info) );
}
