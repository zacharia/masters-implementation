#!/bin/bash
#A script to run the voxel prototype program with the standard parameters.

gdb --args ./voxel_generator -s 128 -g 0.5 --scale-shapes -f ~/temp/output -v -x $@