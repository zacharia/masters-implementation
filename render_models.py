#!/bin/python

import os
import time
import subprocess
import sys

#global_args = "blender -S scene -F JPEG "
global_args = " -f 1 -F PNG "

input_blend_directory = "/home/zcrumley/temp/model_rendering/"
output_picture_directory = "/home/zcrumley/temp/rendering_output_png/"

run_commands = True

to_render = [
             "castles_repositioned",
             "space_stations_repositioned",
             "tanks_repositioned"
             # "castles_128",
             # "c_station",
             # "enterprise_camoflauge",
             # "enterprise_enterprise",
             # "enterprise_rule110",
             # "flat_rectangle_rule30",
             # "flat_rectangle_rule90",
             # "fractal_tree",
             # "skyscraper",
             # "space_stations_128",
             # "spike_ship",
             # "spiral",
             # "tanks_128",
             # "smoothing_comparison_unsmoothed",
             # "smoothing_comparison_smoothed",
             # "spike_ship_redux",
             # "enterprise_rule110_redux",
             # "enterprise_flames"
             ]

for name in to_render:
    print "============================================================"
    print "Rendering input: %s" % (name)
    time_now = time.strftime("%H:%M:%S")
    print "starting at time: %s " % time_now
    
    output_file = output_picture_directory + name
    input_file = input_blend_directory + name + ".blend"

    command = "blender " + " -b " + input_file + " -o " + output_file + ".png" + global_args

    print "Command: %s" % command

    if run_commands:
        sys.stdout.flush()
        p = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
        out,err = p.communicate()
        print out

    time_now = time.strftime("%H:%M:%S")
    print "it ended at time: %s " % time_now
