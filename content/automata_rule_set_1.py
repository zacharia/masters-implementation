num_iterations = 1
neighbourhood_size = 1

def main(voxel, neighbourhood):
    ret = []
    ret.append("grey")
    if voxel["aggregate_normal"][1] > 0:
        ret.append("colour 0.75 0.0 0 0.5")
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
