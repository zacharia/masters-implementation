num_iterations = 1
neighbourhood_size = 1

def main(voxel, neighbourhood, position, octree_size):
    ret = []
    ret.append("grey")
    if position[1] > 90:
        ret.append("position_offset 5 0 5")
        ret.append("material basic/cylinder_default")
        #ret.append("normalize_normals")
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
