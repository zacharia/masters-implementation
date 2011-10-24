#!/bin/bash

# This runs performance tests on the program.

#these are the commands to run.

#if these variables are anything but "true", they don't get done
DO_TIMING=true
DO_MEM_USAGE=false

#this is applied to all commands, for both timing or memory
GLOBAL_ARGS=-st -s 32

TESTS=(
    './voxel_generator -s 128 -g 0.5 --scale-shapes -f /home/zcrumley/temp/enterprise.output -r automata_rule_set_1 -m camoflauge -o content/enterprise.mesh -st -s 512'
    './voxel_generator -s 128 -g 0.5 --scale-shapes -f /home/zcrumley/temp/spike_ship.output -r automata_rule_set_1 -m normal_lightness -o content/spike_ship.mesh -st -s 512'
    './voxel_generator -s 128 -g 0.5 --scale-shapes -f /home/zcrumley/temp/space_station.output -r automata_rule_set_1 -m spiral_detail -o content/space_station.mesh -st -s 512'
    './voxel_generator -s 128 -g 0.5 --scale-shapes -f /home/zcrumley/temp/c_station.output -r automata_rule_set_1 -m c_station_detail -o content/c_station.mesh -st -s 512'
)

echo "================================================================================================="

#this runs the commands and redirects all their output to a file with the same name as the command.
count=0
while [ "x${TESTS[count]}" != "x" ]
do
    # list the command and time it started
    echo -e "\n**************************Running the following command: ${TESTS[count]}"
    echo "**************************at time:"
    date
    echo -e "\n\n"

    #run a timing test if so specified
    if [ ${DO_TIMING} == "true" ]
    then
	echo -e "******************************TIMING:\n\n"
        #run the command
	${TESTS[count]} ${GLOBAL_ARGS}
    else
	echo "*****************************timing was not done"
    fi

    #run a memory usage test if so specified.
    if [ ${DO_MEM_USAGE} == "true" ]
    then
	echo -e "\n\n******************************MEMORY USAGE:\n\n"
        #run the command
	valgrind --tool=massif --massif-out-file=temp.massif --threshold=100.0 --depth=2 ${TESTS[count]} ${MEM_USAGE_GLOBAL_ARGS}
	ms_print temp.massif
	rm temp.massif
    else
	echo -e "\n\n*****************************memory usage was not done"
    fi

    #make a note of when it stopped
    echo -e "\n\n\n**************************the program finished at:"
    date
    echo -e "\n\n\n=================================================================================================\n\n"

    #increase the counter
    count=$(( $count + 1 ))
done