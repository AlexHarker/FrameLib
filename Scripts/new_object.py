
from Project_Utilities import file_util
from Project_Utilities.object_info import fl_object
from Project_Utilities.vs_util import fl_solution
from Project_Utilities.xcode_util import fl_pbxproj
from Project_Utilities.path_util import fl_paths

import os
    
    
def insert_code(contents: str, class_name: str, category: str, path: str, start: str, end: str, insert_end: str, exp: str, layout: str = ""):

    category_compare = category.replace("_", " ")
    categories = file_util.lines_regex(path, "^.*// (.*)", start, end, start, end)
    
    for index, c in enumerate(categories):
        if c[0] == category_compare:
            objects = file_util.lines_regex(path, exp, start, end, "// " + category_compare, "//")
            
            for o in objects:
                if o[0] > class_name:
                    file_util.insert(path, contents, [start, o[1]])
                    return
            
            file_util.insert(path, contents, [start, categories[index][1]], True)
            return
            
    contents = layout + "// " + category_compare + "\n\n" + contents + "\n"

    for c in categories:
        if c[0] > category_compare:
            file_util.insert(path, contents, [start, layout + "// " + c[0]])
            return
    
    file_util.insert(path, contents, [start, insert_end])
    
    
def insert_cpp_single_build(base_class: str, class_name: str, object_info: fl_object, path: str, start: str, end: str):
    contents = "    " + base_class + "<" + object_info.object_class + ">::makeClass(\"" + class_name + "\");\n"
    insert_code(contents, class_name, object_info.category, path, start, end, "    // Unary Operators", "^.*?(fl.*~).*", "    ")
    
    
def insert_object_list_include(paths: fl_paths, object_class: str, category: str):

    path = paths.objects_export_header()
    contents = "#include \"../FrameLib_Objects/" + category + "/" + object_class + ".h\"\n"
    exp = "^.*/FrameLib_Objects/" + category + "/(.*)\.h.*"

    insert_code(contents, object_class, category, path, "#ifndef", "#endif", "// Operators", exp)
    

def main():

    object_info =  fl_object("FrameLib_Object", "fl.testclass~", "Test_Category")
    
    paths = fl_paths()
    vs_solution = fl_solution()
    pbxproj = fl_pbxproj()
    
    object_info.guid = vs_solution.create_guid()

    os.makedirs(paths.object_dir(object_info), exist_ok = True)
    os.makedirs(paths.max_object_dir(object_info), exist_ok = True)
    
    file_util.create(paths.max_source(object_info), paths.template("fl.class_name~.cpp"), object_info)
    file_util.create(paths.object_header(object_info), paths.template("FrameLib_Class.h"), object_info)
    file_util.create(paths.object_source(object_info), paths.template("FrameLib_Class.cpp"), object_info)
    file_util.create(paths.vs_max_project(object_info), paths.template("fl.class_name~.vcxproj"), object_info)
    
    insert_cpp_single_build("FrameLib_MaxClass_Expand", object_info.max_class_name, object_info, paths.max_framelib(), "main(", "}")
    insert_cpp_single_build("FrameLib_PDClass_Expand", object_info.pd_class_name, object_info, paths.pd_framelib(), "framelib_pd_setup(", "}")
    insert_object_list_include(paths, object_info.object_class, object_info.category)
    
    vs_solution.update(object_info, True)
    pbxproj.update(object_info, True)
    
    #vs_solution.update_project(paths.vs_max_project(object_info), False)
    #vs_solution.update_all_projects(False)
    #vs_solution.update_all_projects(True)
    
    
if __name__ == "__main__":
    main()
