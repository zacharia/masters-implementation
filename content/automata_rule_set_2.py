num_iterations = 1
neighbourhood_size = 1

def main(voxel, neighbourhood, position, octree_size):
    ret = []
    ret.append("material basic/vertex_colour_lighting")
    #if position[1] % 2 == 0:
    ret.append("colour from_normal")
    #else:
    #    ret.append("colour 1 1 0 0")
        
    

    #if position[1] > 63:
    #if voxel["aggregate_normal"][1] > 0:
        #ret.append("position_offset 0 5 0")
        #ret.append("material basic/cylinder_default")
        #ret.append("normalize_normals")
    #else:
        #ret.append("material basic/textureexample")
        #ret.append("position_offset 0 0 -5")

    # import code
    # code.interact(local=locals())

        
    return ret
