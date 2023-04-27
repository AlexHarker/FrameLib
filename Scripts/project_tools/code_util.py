
from . file_util import rw_file
from . object_info import fl_object
from . path_util import fl_paths

from . import file_util


# Get a list of items from a string with the specified bounds based on a regex

def item_list(data: str, exp: str, bounds: list):
    
    import re
    
    regex = re.compile(exp)
    list = []
    s1 = False
    s2 = False
    started = False
    
    for line in data.splitlines():
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


# Insert code into a string based on the given bounds and regex for finding items

def insert_code(data: str, contents: str, class_name: str, info: fl_object, bounds: list, exp: str, layout: str = ""):

    # Core variables
    
    category = info["category"]
    category_compare = category.replace("_", " ")
    categories = item_list(data, "^.*// (.*)", [bounds[0], bounds[1], bounds[0], bounds[1]])
    
    # Loop over existent categories
    
    for index, c in enumerate(categories):
    
        # If the category exists find where to insert the item
        
        if c[0] == category_compare:
        
            # Get a list of objects
            
            objects = item_list(data, exp, [bounds[0], bounds[1]] + ["// " + category_compare, "//"])
            
            # If the item should precede an existing one insert it there
            
            for o in objects:
                if o[0] > class_name:
                    return file_util.insert(data, contents, [bounds[0], o[1]])
            
            # Otherwise insert at the final position

            return file_util.insert(data, contents, [bounds[0], categories[index][1]], True)
            
    # If the category doesn't exist it will require creating
    
    contents = layout + "// " + category_compare + "\n\n" + contents + "\n"

    # If the category should precede an existing one insert it there
    
    for c in categories:
        if c[0] > category_compare:
            return file_util.insert(data, contents, [bounds[0], layout + "// " + c[0]])
            
    # Otherwise insert at the final position
    
    return file_util.insert(data, contents, [bounds[0], bounds[2]])
    
    
# Insert code into the cpp file for a single build version of framelib
    
def update_single_build(path: str, template: str, class_name: str, info: fl_object, bounds: list, warn: bool):
   
    file = rw_file(path)
    
    contents = file_util.templated_string(fl_paths().code_template(template), info)
    exp = "^.*?(fl.*~).*"
    
    if file.data.find(contents) < 0:
        file.data = insert_code(file.data, contents, class_name, info, bounds + ["    // Unary Operators"], exp, "    ")
        file.flush()
        
        if warn:
            print("\nADDED CODE TO " + path.rsplit("/", 1)[1] + "\n")
    
    
# Insert code into the header file that lists all framelib objects

def update_object_list_include(info: fl_object, warn: bool):

    object_class = info["object_class"]
    contents = file_util.templated_string(fl_paths().code_template("object_list_include"), info)
    exp = "^.*/FrameLib_Objects/" + info["category"] + "/(.*)\.h.*"

    file = rw_file(fl_paths().objects_export_header())

    if file.data.find(contents) < 0:
        file.data = insert_code(file.data, contents, object_class, info, ["#ifndef", "#endif", "// Operators"], exp)
        file.flush()
        
        if warn:
            print("\nADDED CODE TO " + path.rsplit("/", 1)[1] + "\n")
    
    
# Update the code for the given object

def update_code(info: fl_object, warn: bool):

    max_build_path = fl_paths().max_framelib()
    pd_build_path = fl_paths().pd_framelib()

    if info["object_class"] == "void":
        return
        
    if info["max_host_class"] == "FrameLib_MaxClass_Expand" or info["max_host_class"] == "FrameLib_MaxClass":
        max_template = "single_build_max"
        pd_template = "single_build_pd"
    else:
        max_template = "single_build_max_custom"
        pd_template = "single_build_pd_custom"
    
    update_single_build(max_build_path, max_template, info["max_class_name"], info, ["main(", "}"], warn)
    update_single_build(pd_build_path, pd_template, info["pd_class_name"], info, ["framelib_pd_setup(", "}"], warn)
    
    if fl_paths().object_header_exists(info):
        update_object_list_include(info, warn)
