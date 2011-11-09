#!/bin/python

import os
import time
import subprocess
import sys

sizes = [64, 128, 256, 512]
#sizes = [64, 128]
global_args = "./voxel_generator -g 0.5 --scale-shapes -st -sm -r automata_rule_set_1 "
input_files_directory = "/home/zcrumley/Documents/Masters/mesh_prototype/content/shape_grammar_outputs/"
mesh_output_directory = "/home/zcrumley/temp/extra_testing_mesh_output/"

run_commands = True

#[".output", "", ""],

models = [
    
          # ["castle_1.output", "castle", "5"],
          # ["castle_2.output", "castle", "5"],
          # ["castle_3.output", "castle", "5"],
          # ["castle_4.output", "castle", "5"],
          # ["castle_5.output", "castle", "5"],
          # ["castle_6.output", "castle", "5"],
          # ["castle_7.output", "castle", "5"],
          # ["castle_8.output", "castle", "5"],

          # ["tank_1.output", "tank", "1"],
          # ["tank_2.output", "tank", "1"],
          # ["tank_3.output", "tank", "1"],
          # ["tank_4.output", "tank", "1"],
          # ["tank_1.output", "tank", "1"],
          # ["tank_6.output", "tank", "1"],
          # ["tank_7.output", "tank", "1"],
          # ["tank_8.output", "tank", "1"],

          # ["space_station_1.output", "c_station_detail", "5"],
          # ["space_station_2.output", "c_station_detail", "5"],
          # ["space_station_3.output", "c_station_detail", "5"],
          # ["space_station_4.output", "c_station_detail", "5"],
          # ["space_station_5.output", "c_station_detail", "5"],
          # ["space_station_6.output", "c_station_detail", "5"],
          # ["space_station_7.output", "c_station_detail", "5"],
          # ["space_station_8.output", "c_station_detail", "5"],
          # ["space_station_9.output", "c_station_detail", "5"],
          # ["space_station_10.output", "c_station_detail", "5"],

          # ["skyscraper.output", "skyscraper", "1"],
          
          # ["enterprise.output", "enterprise", "1"],
          # ["enterprise.output", "camoflauge", "1"],
          # ["enterprise.output", "rule110", "20"],
          
          # ["fractal_tree.output", "grey", "1"],
          
          # ["spiral.output", "spiral_detail", "1"],
          
          # ["spike_ship.output", "spaceship_windows", "1"],

          # ["c_station.output", "c_station_detail", "5"],

          # ["flat_rectangle.output", "rule90", "25"],
          # ["flat_rectangle.output", "rule30", "25"],

          #some extra setups I added for the journal application thing.

          ["enterprise.output", "flames", "15"],
          #["spike_ship.output", "spike_ship", "15"],
          
          ]

#loop over all the sizes
for grid_size in sizes:
    #loop over each model
    for i in range(len(models)):
        print "====================================================================================="
        #this is the name of the output mesh file that we'll use
        output_name = "%s_%s_%s_%i.mesh" % (models[i][0], models[i][1], models[i][2], grid_size)
        #this is the command to run for this particular test.
        command = global_args + " -s " + str(grid_size) + " -f " + input_files_directory + models[i][0] + " -m " + models[i][1] + " -ani " + models[i][2] + " -o " + mesh_output_directory + output_name
        #output some information
        time_now = time.strftime("%H:%M:%S")
        print "at time: %s" % time_now
        print "we ran the command: ||| %s |||" % command

        #os.system(command)

        #this was a replacement for the above that I got from Stefano.
        if run_commands:
            sys.stdout.flush()
            p = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
            out,err = p.communicate()
            print out

        time_now = time.strftime("%H:%M:%S")
        print "it finished at time: %s" % time_now
        
        print "====================================================================================="


