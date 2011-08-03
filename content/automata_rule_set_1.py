num_iterations = 1
neighbourhood_size = 1

def main(voxel, neighbourhood, position, octree_size):
    ret = []
    #if position[1] % 2 == 0:
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

    #ret.append("colour from_normal")

    if "ring" in voxel["tags"]:
        ret.append("colour 1.0 0 0 0.5")
        ret.append("material basic/vertex_colour_lighting")
        pass
    elif "spire" in voxel["tags"]:
        #ret.append("colour 0 1 0 1")
        if position[1] % 3 == 0:
            ret.append("material spaceship/metal_wall")
        else:
            ret.append("material spaceship/window_ugly")
        pass
    elif "spoke" in voxel["tags"]:
        #ret.append("colour 0 0 1 1")
        ret.append("material basic/vertex_colour_lighting")
        pass
    elif "tower" in voxel["tags"]:
        #ret.append("colour 1 0 1 0.5")
        ret.append("material basic/vertex_colour_lighting")
        pass
    elif "centre" in voxel["tags"]:
        #ret.append("colour 0 0 0 0.75")
        ret.append("material basic/vertex_colour_lighting")
        pass
    else:
        #ret.append("colour 0 0 0 0.75")
        ret.append("material basic/vertex_colour_lighting")
        pass
    
    return ret
