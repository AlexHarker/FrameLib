from . import file_util
from . object_info import fl_object
from . path_util import fl_paths


# Get a list of items from a file with the specified bounds based on a regex

def item_list(path: str, exp: str, bounds: list):
    
    import re
    
    regex = re.compile(exp)
    list = []
    s1 = False
    s2 = False
    started = False
    
    with open(path) as f:
        for line in f:
            match = regex.search(line)
            
            if bounds[0] in line:
                s1 = True
            if bounds[2] in line:
                s2 = True
              
            if started and (bounds[1] in line or bounds[3] in line):
                return list
                
            started = s1 and s2
            
            if started and match is not None:
                list.append([match.group(1), match.group(0)])

    return list


# Insert code into a file between

def insert_code(path: str, contents: str, class_name: str, info: fl_object, bounds: list, exp: str, layout: str = ""):

    # Core variables
    
    category = info["category"]
    category_compare = category.replace("_", " ")
    categories = item_list(path, "^.*// (.*)", [bounds[0], bounds[1], bounds[0], bounds[1]])
    
    # Loop over existent categories
    
    for index, c in enumerate(categories):
    
        # If the category exists find where to insert the item
        
        if c[0] == category_compare:
        
            # Get a list of objects
            
            objects = item_list(path, exp, [bounds[0], bounds[1]] + ["// " + category_compare, "//"])
            
            # If the item should precede an existing one insert it there
            
            for o in objects:
                if o[0] > class_name:
                    file_util.insert_file(path, contents, [bounds[0], o[1]])
                    return
            
            # Otherwise insert at the final position

            file_util.insert_file(path, contents, [bounds[0], categories[index][1]], True)
            return
            
    # If the category doesn't exist it will require creating
    
    contents = layout + "// " + category_compare + "\n\n" + contents + "\n"

    # If the category should precede an existing one insert it there
    
    for c in categories:
        if c[0] > category_compare:
            file_util.insert_file(path, contents, [bounds[0], layout + "// " + c[0]])
            return
    
    # Otherwise insert at the final position
    
    file_util.insert_file(path, contents, [bounds[0], bounds[2]])
    
    
# Insert code into the cpp file for a single build version of framelib
    
def insert_single_build(path: str, template: str, class_name: str, info: fl_object, bounds: list):
   
    contents = file_util.templated_string(fl_paths().code_template(template), info)
    exp = "^.*?(fl.*~).*"
    insert_code(path, contents, class_name, info, bounds + ["    // Unary Operators"], exp, "    ")
    
    
# Insert code into the header file that lists all framelib objects

def insert_object_list_include(info: fl_object):

    path = fl_paths().objects_export_header()
    contents = file_util.templated_string(fl_paths().code_template("object_list_include"), info)
    exp = "^.*/FrameLib_Objects/" + info["category"] + "/(.*)\.h.*"
    insert_code(path, contents, info["object_class"], info, ["#ifndef", "#endif", "// Operators"], exp)
    
    
# Update the code for the given object

def update_code(info: fl_object):

    max_build_path = fl_paths().max_framelib()
    pd_build_path = fl_paths().pd_framelib()

    insert_single_build(max_build_path, "single_build_max", info["max_class_name"], info, ["main(", "}"])
    insert_single_build(pd_build_path, "single_build_pd", info["pd_class_name"], info, ["framelib_pd_setup(", "}"])
    insert_object_list_include(info)
