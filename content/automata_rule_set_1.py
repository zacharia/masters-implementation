import random

num_iterations = 1
neighbourhood_size = 1

def space_station_detail(voxel, neighbourhood, position, octree_size, curr_iteration):
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
        ret.append("colour 0 1 0 1")
        # if position[1] % 3 == 0:
        #     ret.append("material spaceship/metal_wall")
        # else:
        #     ret.append("material spaceship/window_ugly")
        pass
    elif "spoke" in voxel["tags"]:
        ret.append("colour 0 0 1 1")
        ret.append("material basic/vertex_colour_lighting")
        pass
    elif "tower" in voxel["tags"]:
        ret.append("colour 1 0 1 0.5")
        ret.append("material basic/vertex_colour_lighting")
        pass
    elif "centre" in voxel["tags"]:
        ret.append("colour 0 0 0 0.75")
        ret.append("material basic/vertex_colour_lighting")
        pass
    else:
        ret.append("colour 0 0 0 0.75")
        ret.append("material basic/vertex_colour_lighting")
        pass
    
    return ret


def spiral_detail(voxel, neighbourhood, position, octree_size, curr_iteration):
    ret = []

    ret.append("material basic/vertex_colour_lighting")
    if position[1] % 10 < 5:
        ret.append("colour 1 0 0 0")
    else:
        ret.append("colour 0 1 0 0")

    return ret


def fractal_tree_detail(voxel, neighbourhood, position, octree_size, curr_iteration):
    ret = []

    ret.append("material basic/scrolling_metal_wall")
    
    # import code
    # code.interact(local=locals())
    # rand_num = random.randint(1,3)
    # if rand_num == 1:
    #     ret.append("colour 0 0 1 0")
    # elif rand_num == 2:
    #     ret.append("colour 0 1 1 0")
    # elif rand_num == 3:
    #     ret.append("colour 0 0 0 0")

    return ret

def colour_from_normals(voxel, neighbourhood, position, octree_size, curr_iteration):
    ret = []
    ret.append("material basic/vertex_colour_lighting")
    
    ret.append("colour from_normal")
        
    return ret

def rule90(voxel, neighbourhood, position, octree_size, curr_iteration):
    ret = []

    if position[0] == 64 and position[1] == 100:
        ret.append("on")

    upper_row = ["on" in neighbourhood[0][2][1]["detail_info"], "on" in neighbourhood[1][2][1]["detail_info"], "on" in neighbourhood[2][2][1]["detail_info"]]
    result = True

    if upper_row[0]:
        if upper_row[1]:
            if upper_row[2]:
                result = False
            else:
                result = True
        else:
            if upper_row[2]:
                result = False
            else:
                result = True
    else:
        if upper_row[1]:
            if upper_row[2]:
                result = True
            else:
                result = False
        else:
            if upper_row[2]:
                result = True
            else:
                result = False

    if result:
        ret.append("on")
        
    if "on" in voxel["detail_info"]:
        ret.append("material basic/vertex_colour_lighting")
        ret.append("colour from_normal")
        
    return ret

def enterprise(voxel, neighbourhood, position, octree_size, curr_iteration):
    ret = []

    base_grey = "material brick/concrete"
    window = "material spaceship/window_ugly"
    glowing_blue = "material basic/blue"
    glowing_red = "material basic/red"

    if "disk" in voxel["tags"]:
        if abs(voxel["aggregate_normal"][1]) < 0.6 and position[1] % 2 == 1 and random.randint(1,4) == 1:
            ret.append(window)
        else:
            ret.append(base_grey)

    elif "neck" in voxel["tags"]:
        if abs(voxel["aggregate_normal"][1]) == 0 and position[1] % 3 == 1 and random.randint(1,4) == 1:
            ret.append(window)
        else:
            ret.append(base_grey)
    
    elif "body" in voxel["tags"]:
        if abs(voxel["aggregate_normal"][1]) < 0.5 and abs(voxel["aggregate_normal"][2]) == 0 and position[1] % 4 == 1 and random.randint(1,4) == 1:
            ret.append(window)
        else:
            ret.append(base_grey)

    elif "thruster_bumps" in voxel["tags"]:
        ret.append(glowing_blue)

    elif "thruster" in voxel["tags"]:
        if  abs(voxel["aggregate_normal"][2]) == 1:
            ret.append(glowing_red)
        else:
            ret.append(base_grey)

    elif "wing" in voxel["tags"]:
        ret.append(base_grey)

    elif "body_extension" in voxel["tags"]:
        ret.append(glowing_blue)

    #ret.append(voxel["detail_info"][0])
    
    return ret

def normal_lightness(voxel, neighbourhood, position, octree_size, curr_iteration):
    ret = []

    ret.append("material basic/vertex_colour_lighting")
    value = abs(voxel["aggregate_normal"][1])
    ret.append("colour %f %f %f 0" % (value, value, value))

    return ret


def skyscraper(voxel, neighbourhood, position, octree_size, curr_iteration):
    ret = []

    window_material = "material basic/blue"
    wall_material = "material brick/concrete"

    if "window_pane" in voxel["tags"]:
        ret.append(window_material)
    elif "wall" in voxel["tags"]:
        ret.append(wall_material)
    
    return ret


# this works nicely, and isn't tag dependent.
def camoflauge(voxel, neighbourhood, position, octree_size, curr_iteration):
    ret = []
    
    num = random.randint(1,20)
    if num == 1:
        ret.append("material basic/brown")
    elif num == 2:
        ret.append("material nature/grass")
    elif num == 3:
        ret.append("material basic/dark_green")
        
    return ret


# this doesn't work completely yet. I'm ironing out some bugs.
def tank(voxel, neighbourhood, position, octree_size, curr_iteration):
    ret = []

    if "tread" in voxel["tags"]:
        ret.append("basic/red")

    elif "gun_tip" in voxel["tags"] or  "gun" in voxel["tags"]:
        ret.append("basic/red")

    else:
        num = random.randint(1,20)
        if num == 1:
            ret.append("material basic/brown")
        elif num == 2:
            ret.append("material nature/grass")
        elif num == 3:
            ret.append("material basic/dark_green")
        
    return ret


def c_station_detail(voxel, neighbourhood, position, octree_size, curr_iteration):
    ret = []

    metal_main = "material metal/diamond_plate"
    metal_spots = "material metal/hole_plate"
    windows = "material spaceship/window_ugly"

    if curr_iteration == 1:
      if random.randint(1,100) == 1:
          ret.append(metal_spots)
      else:
          ret.append(metal_main)
      
    elif curr_iteration > 1 and curr_iteration <= 4:
        
        been_set = False
        
        for i in neighbourhood:
            for j in i:
                for k in j:
                    if metal_spots in k["detail_info"]:
                        ret.append(metal_spots)
                        been_set = True

        if not been_set:
            ret.append(metal_main)

    elif curr_iteration == 5:
        if abs(voxel["aggregate_normal"][1]) == 0 and position[1] % 3 == 1 and random.randint(1,4) == 1:
            ret.append(windows)
        else:
            ret.append(voxel["detail_info"][0])

    # {'aggregate_tags': ['wing'], 'tags': ['wing'], 'solid': True, 'detail_info': ['material basic/green'], 'aggregate_solid': True, 'aggregate_normal': [-0.7071067690849304, -0.7071067690849304, 0.0]}
        
    # if "central_spire" in voxel["tags"]:
    #     if position[1] % 3 == 1 and random.randint(1,4) == 1:
    #             ret.append("material basic/blue")
    #     else:
    #         if random.randint(1,2) == 1:
    #             ret.append("material metal/diamond_plate")
    #         else:
    #             ret.append("material metal/hole_plate")
        
    # if "wing" in voxel["tags"]:
    #     if random.randint(1,2) == 1:
    #         ret.append("material metal/diamond_plate")
    #     else:
    #         ret.append("material metal/hole_plate")

    # if "wing" in voxel["tags"] and abs(voxel["aggregate_normal"][1]) == 0:
    #     ret.append("material basic/red")

    # for i in neighbourhood:
    #     for j in i:
    #         for k in j:
    #             if "indent" in k["tags"]:
    #                 ret.append("material basic/red")

    # import code
    # code.interact(local=locals())
        
    return ret


# this is constantly changing method I use for seeing how textures look.
def testing(voxel, neighbourhood, position, octree_size, curr_iteration):
    return ["material testing"]


# this one makes the entire thing covered in metal and with windows.
def spaceship_windows(voxel, neighbourhood, position, octree_size, curr_iteration):
    ret = []
    
    if abs(voxel["aggregate_normal"][1]) == 0 and position[1] % 3 == 1 and random.randint(1,4) == 1:
        ret.append("material basic/blue")
    else:
        if random.randint(1,2) == 1:
            ret.append("material metal/diamond_plate")
        else:
            ret.append("material metal/hole_plate")

    return ret


# this makes each shape that has the tag "col_<num>" a random shade of grey corresponding to num
#in the shape grammar, assign the random colours with this code:

# value = random.random()
# the_tag = "col_%f" % value
# parent.tags.append(the_tag)

def random_grey(voxel, neighbourhood, position, octree_size, curr_iteration):
    ret = []

    ret.append("material basic/vertex_colour_lighting")
    value = 0;

    for i in voxel["tags"]:
        stuff = i.split("_")
        if stuff[0] == "col":
            value = float(stuff[1])

    ret.append("colour %f %f %f 0" % (value, value, value))
    
    return ret

def castle(voxel, neighbourhood, position, octree_size, curr_iteration):
    ret = []

    stone_main = "material testing"
    stone_spots = "material basic/dark_green"
    stone_small_spots = "material brick/concrete"
    floor = "material basic/red"

    if curr_iteration == 1:
        if random.randint(1,100) == 1:
            ret.append(stone_spots)
        else:
            ret.append(stone_main)

    elif curr_iteration == 2:
        if abs(voxel["aggregate_normal"][1]) == 0 and position[1] % 3 == 1 and random.randint(1,4) == 1:
            ret.append(stone_small_spots)
        else:
            ret.append(voxel["detail_info"][0])    
            
    elif curr_iteration >= 3 and curr_iteration <= 5:
        
        been_set = False
        
        for i in neighbourhood:
            for j in i:
                for k in j:
                    if stone_spots in k["detail_info"] and random.randint(1,3) == 1:
                        ret.append(stone_spots)
                        been_set = True

        if not been_set:
            ret.append(voxel["detail_info"][0])

        #this was for getting the floor to be different. It needs more tinkering, and I've given up on it for now.
        #if abs(voxel["aggregate_normal"][1]) == 1 and ("castle_wall" in voxel["tags"] or "castle_tower" in voxel["tags"]):
            #ret.append(floor)
        
    return ret
