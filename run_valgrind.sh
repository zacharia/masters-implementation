#!/bin/bash
#A script to run the voxel prototype program with the standard parameters.

valgrind --leak-check=full ./voxel_generator -s 128 -g 0.5 --scale-shapes -f ~/temp/output -v -x -r automata_rule_set_1 $@ 2>&1 | less