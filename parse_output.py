import fileinput

output_file_name = "/home/zcrumley/temp/results.csv"
out = open(output_file_name, 'w')

def process(line):
    if line != "\n":
        split_line = line.split()

        if split_line[0] == "at_time:":
            out.write(split_line[1] + ",")
        elif split_line[0] == "ran_command:":
            out.write(split_line[11] + ",")
        elif split_line[0] == "voxelization_time":
            out.write(split_line[1] + ",")
        elif split_line[0] == "peak_memory_usage_after_voxelization:":
            out.write(split_line[1] + ",")
        elif split_line[0] == "voxel_detailing_time":
            out.write(split_line[1] + ",")
        elif split_line[0] == "Peak_memory_usage_after_voxel_detailing_was:":
            out.write(split_line[1] + ",")
        elif split_line[0] == "meshing_voxel_grid_time":
            out.write(split_line[1] + ",")
        elif split_line[0] == "Peak_memory_usage_after_meshing_was:":
            out.write(split_line[1] + ",")
        elif split_line[0] == "Peak_memory_usage_was:":
            out.write(split_line[1] + ",")
        elif split_line[0] == "finished_at_time:":
            out.write(split_line[1] + ",\n")

out.write("start time,size,voxelization time (ms),mem after voxelization (kB),voxel detailing time (ms),mem after detailing,meshing time (ms),mem after meshing,peak mem,finish time,\n")

for line in fileinput.input():
    process(line)

out.close()

print "file %s was created successfully" % (output_file_name)
