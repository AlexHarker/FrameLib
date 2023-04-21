
from Project_Utilities import file_util
from Project_Utilities.vs_util import fl_solution
from Project_Utilities.path_util import fl_paths

import os

    
def single_build_cpp(base_class: str, object_class: str, class_name: str):
    return "    " + base_class + "<" + object_class + ">::makeClass(\"" + class_name + "\");\n"
    
    
def insert_code(contents: str, class_name: str, category: str, path: str, start: str, end: str, insert_end: str, exp: str, layout: str = ""):

    category_compare = category.replace("_", " ")
    categories = file_util.lines_regex(path, "^.*// (.*)", start, end, start, end)
    
    for index, c in enumerate(categories):
        if c[0] == category_compare:
            objects = file_util.lines_regex(path, exp, start, end, "// " + category_compare, "//")
            
            for o in objects:
                if o[0] > class_name:
                    file_util.insert(path, contents, start, o[1])
                    return
            
            file_util.insert(path, contents, start, categories[index][1], True)
            return
            
    contents = layout + "// " + category_compare + "\n\n" + contents + "\n"

    for c in categories:
        if c[0] > category_compare:
            file_util.insert(path, contents, start, layout + "// " + c[0])
            return
    
    file_util.insert(path, contents, start, insert_end)
    

def insert_cpp_single_build(class_name: str, contents: str, category: str, path: str, start: str, end: str):
    insert_code(contents, class_name, category, path, start, end, "    // Unary Operators", "^.*?(fl.*~).*", "    ")
    

def insert_object_list_include(paths: fl_paths, object_class: str, category: str):

    path = paths.objects_export_header()
    contents = "#include \"../FrameLib_Objects/" + category + "/" + object_class + ".h\"\n"
    exp = "^.*/FrameLib_Objects/" + category + "/(.*)\.h.*"

    insert_code(contents, object_class, category, path, "#ifndef", "#endif", "// Operators", exp)
    
    
def add_xcode_target(object_class: str, class_name: str, category: str):
    #add target
    #setup coorectly for path
    #change/set source files
    #add target as a dependecy for the max objects build
    return ""


def main():

    category = "Test_Category"
    object_class = "FrameLib_Object"
    class_name = "fl.testclass~"
    
    paths = fl_paths(category)
    vs_solution = fl_solution()
    guid = vs_solution.create_guid()

    os.makedirs(paths.object_dir, exist_ok = True)
    os.makedirs(paths.max_object_dir, exist_ok = True)
    
    file_util.create(paths.template("fl.class_name~.cpp"), paths.max_source(class_name), object_class, class_name, category, guid)
    file_util.create(paths.template("FrameLib_Class.h"), paths.object_header(object_class), object_class, class_name, category, guid)
    file_util.create(paths.template("FrameLib_Class.cpp"), paths.object_source(object_class), object_class, class_name, category, guid)
    file_util.create(paths.template("fl.class_name~.vcxproj"), paths.vs_max_project(class_name), object_class, class_name, category, guid)

    max_cpp = single_build_cpp("FrameLib_MaxClass_Expand", object_class, class_name)
    pd_cpp = single_build_cpp("FrameLib_PDClass_Expand", object_class, class_name)
    
    insert_cpp_single_build(class_name, max_cpp, category, paths.max_framelib(), "main(", "}")
    insert_cpp_single_build(class_name, pd_cpp, category, paths.pd_framelib(), "framelib_pd_setup(", "}")
    insert_object_list_include(paths, object_class, category)
    
    vs_solution.update(object_class, class_name, category, guid, True)
    
    add_xcode_target(object_class, class_name, category)
    
    #vs_solution.update_project(paths.vs_max_project(class_name), False)
    #vs_solution.update_all_projects(False)
    
    
if __name__ == "__main__":
    main()
