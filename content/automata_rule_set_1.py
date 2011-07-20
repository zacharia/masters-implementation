num_iterations = 1
neighbourhood_size = 1

def main(voxel, neighbourhood, position, octree_size):
    ret = []
    if position[1] % 2 == 0:
    #if voxel["aggregate_normal"][1] > 0:
        #ret.append("position_offset 0 0 5")
        ret.append("material basic/cylinder_default")
        #ret.append("normalize_normals")
    else:
        ret.append("material basic/cube_default")
        #ret.append("position_offset 0 0 -5")

    # import code
    # code.interact(local=locals())
    # for i in range(-neighbourhood_size, neighbourhood_size + 1):
    #     for j in range(-neighbourhood_size, neighbourhood_size + 1):
    #         for k in range(-neighbourhood_size, neighbourhood_size + 1):
    #             print i,j,k,
    #             print neighbourhood[i][j][k],
    #             print " ",
    #         print "\n"
    #     print "\n\n"
    # print "=================================================\n\n"
    return ret
