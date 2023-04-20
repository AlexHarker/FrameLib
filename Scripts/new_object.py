import os
import uuid
import re
from pathlib import Path


def get_framelib_dir():
    return os.path.dirname(os.path.dirname(os.path.abspath(__file__))) + "/"
    
    
def get_vs_project_dir():
    return get_framelib_dir() + "Projects/Visual_Studio/"
    
    
def get_vs_project_path(class_name: str):
    return get_vs_project_dir() + "03_Max_Objects/" + class_name + ".vcxproj"


def get_template_dir():
    return get_framelib_dir() + "Scripts/Resources/"
    
    
def get_template(file_name: str):
    return get_template_dir() + file_name


def create_guid():
    guid = uuid.uuid4()
    return str(guid).upper()
    
    
def get_item_regex(path: str, exp: str):

    regex = re.compile(exp)
    
    with open(path) as f:
        data = f.read()
        match = regex.search(data)
        
        if match is not None:
            return match.group(1)

    return ""
    
    
def get_guid_regex(exp: str):
    return get_item_regex(get_framelib_dir() + "framelib.sln", exp)


def get_file_lines_regex(path: str, exp: str, start: str, end: str):
    
    regex = re.compile(exp)
    list = []
    started = False;
    
    with open(path) as f:
        for line in f:
            match = regex.search(line)
            
            if start in line:
                started = True
                
            if started and end in line:
                return list
            
            if started and match is not None:
                list.append(match.group(1))

    return list
    
    
def get_main_guid():
    return get_guid_regex("Project\(\"\{(.*)\}\"\) = \"framelib\"")


def get_guid(item: str):
    return get_guid_regex("\"" + item + "\".*\{(.*)\}")
    
    
def file_create(template_path: str, output_path: str, object_class: str, class_name: str, category: str, guid: str):

    f = open(template_path, "r")
    template = f.read()
    f.close()

    template = template.replace("_##CLASS##_", object_class)
    template = template.replace("_##CLASS_UPPER##_", object_class.upper())
    template = template.replace("_##CLASSNAME##_", class_name)
    template = template.replace("_##CATEGORY##_", category)
    template = template.replace("_##GUID##_", guid)
    
    f = open(output_path, "x")
    f.write(template)
    f.close()


def file_insert(path: str, contents: str, start: str, end: str):
    
    data = ""
    
    with open(path, "r") as f:
        data = f.read()
        index = data.find(start) + len(start)
        index = data.find(end, index)
        
    with open(path, "w") as f:
        f.write(data[:index] + contents + data[index:])


def file_remove(path: str, contents: str, start: str, end: str):
    
    data = ""
    
    with open(path, "r") as f:
        data = f.read()
        index_start = data.find(start)
        index_end = data.find(end, index_start + len(start))
        index = data.find(contents, index_start, index_end)
        
    if index < 0:
        return
        
    with open(path, "w") as f:
        f.write(data[:index] + data[index + len(contents):])


def single_build_cpp(base_class: str, object_class: str, class_name: str):
    return "    " + base_class + "<" + object_class + ">::makeClass(\"" + class_name + "\");\n"
    
    
def insert_code(contents: str, category: str, path: str, start: str, end: str, insert_end: str, layout: str = ""):

    category_compare = category.replace("_", " ")
    categories = get_file_lines_regex(path, "// (.*)", start, end)
    
    for c in categories:
        if c == category_compare:
            print("Exists " + c)
#get_file_lines_regex(path, ".*(fl.*~)", start, end)
            return
            
    contents = layout + "// " + category_compare + "\n\n" + contents + "\n"

    for c in categories:
        if c > category_compare:
            file_insert(path, contents, start, layout + "// " + c)
            return
    
    file_insert(path, contents, start, insert_end)
    

def insert_cpp_single_build(contents: str, category: str, path: str, start: str, end: str):
    insert_code(contents, category, path, start, end, "    // Unary Operators", "    ")
    

def insert_object_list_include(object_class: str, category: str):

    path = get_framelib_dir() + "FrameLib_Exports/FrameLib_Objects.h"
    contents = "#include \"../FrameLib_Objects/" + category + "/" + object_class + ".h\"\n"

    insert_code(contents, category, path, "#ifndef", "#endif", "#endif")

    
def add_xcode_target(object_class: str, class_name: str, category: str):
    #add target
    #setup coorectly for path
    #change/set source files
    #add target as a dependecy for the max objects build
    return ""


def make_vs_project(object_class: str, class_name: str, category: str, guid: str):
    file_create(get_template("fl.class_name~.vcxproj"), get_vs_project_path(class_name), object_class, class_name, category, guid)
    

def vs_solution_insert(contents: str, start: str, end: str):
    file_insert(get_framelib_dir() + "framelib.sln", contents, start, end)


def vs_solution_insert_remove(contents: str, start: str, end: str, insert: bool):
    if insert:
        file_insert(get_framelib_dir() + "framelib.sln", contents, start, end)
    else:
        file_remove(get_framelib_dir() + "framelib.sln", contents, start, end)
    

def vs_solution_relation(guid: str, guid2: str):
    return "\t\t{" + guid + "} = {" + guid2 + "}\n"
        
 
def vs_object_project_insert_remove(contents: str, start: str, end: str, insert: bool):
    if insert:
        file_insert(get_vs_project_dir() + "02_Common/framelib_objects.vcxproj", contents, start, end)
    else:
        file_remove(get_vs_project_dir() + "02_Common/framelib_objects.vcxproj", contents, start, end)


def vs_solution_add_rm_project(class_name: str, guid: str, fl_guid: str, fl_objects_guid: str, insert: bool):

    contents = "Project(\"{" + get_main_guid() + "}\") = \"" + class_name + "\", \"" + "Projects\Visual_Studio\\03_Max_Objects\\" + class_name + ".vcxproj\", \"{" + guid + "}\"\n\tProjectSection(ProjectDependencies) = postProject\n\t\t{" + fl_guid + "} = {" + fl_guid + "}\n\t\t{" + fl_objects_guid + "} = {" + fl_objects_guid + "}\n\tEndProjectSection\nEndProject\n"
        
    vs_solution_insert_remove(contents, "MinimumVisualStudioVersion", "Global", insert)
  

def vs_solution_add_rm_configurations(guid: str, insert: bool):

    contents = "\t\t{GUID}.Debug|x64.ActiveCfg = Debug|x64\n\t\t{GUID}.Debug|x64.Build.0 = Debug|x64\n\t\t{GUID}.Debug|x86.ActiveCfg = Debug|x64\n\t\t{GUID}.Release|x64.ActiveCfg = Release|x64\n\t\t{GUID}.Release|x64.Build.0 = Release|x64\n\t\t{GUID}.Release|x86.ActiveCfg = Release|x64\n\t\t{GUID}.Static|x64.ActiveCfg = Release|x64\n\t\t{GUID}.Static|x64.Build.0 = Release|x64\n\t\t{GUID}.Static|x86.ActiveCfg = Release|x64\n\t\t{GUID}.Static|x86.Build.0 = Release|x64\n"
        
    contents = contents.replace("GUID", guid)
    
    vs_solution_insert_remove(contents, "GlobalSection(ProjectConfigurationPlatforms) = postSolution", "\tEndGlobalSection", insert)


def vs_solution_add_rm_nesting(guid: str, fl_max_objects_guid: str, insert: bool):

    contents = vs_solution_relation(guid, fl_max_objects_guid)
    vs_solution_insert_remove(contents, "GlobalSection(NestedProjects) = preSolution", "\tEndGlobalSection", insert)


def vs_solution_add_rm_dependency(guid: str, insert: bool):

    contents = vs_solution_relation(guid, guid)
    vs_solution_insert_remove(contents, "\"framelib_objects_max\"", "\tEndProjectSection", insert)


def vs_object_project_add_rm(object_class: str, class_name: str, category: str, guid: str, insert: bool):

    base_path = get_framelib_dir() + "FrameLib_Objects\\" + category + "\\" + object_class;

    if os.path.exists(base_path + ".h"):
        header = "    <ClInclude Include=\"..\..\..\FrameLib_Objects\\" + category + "\\" + object_class + ".h\" />\n"
        vs_object_project_insert_remove(header, "<ClInclude Include", "  </ItemGroup>", insert)
    
    if os.path.exists(base_path + ".cpp"):
        cplusplus = "    <ClCompile Include=\"..\..\..\FrameLib_Objects\\" + category + "\\" + object_class + ".cpp\" />\n"
        vs_object_project_insert_remove(cplusplus, "<ClCompile Include", "  </ItemGroup>", insert)

    
def update_vs_solution(class_name: str, guid: str, add: bool):

    fl_guid = get_guid("framelib")
    fl_objects_guid = get_guid("framelib_objects")
    fl_max_objects_guid = get_guid("Max Object Projects")
    
    vs_solution_add_rm_project(class_name, guid, fl_guid, fl_objects_guid, add)
    vs_solution_add_rm_configurations(guid, add)
    vs_solution_add_rm_nesting(guid, fl_max_objects_guid, add)
    vs_solution_add_rm_dependency(guid, add)
    
    
def add_rm_vs_project(path: str, add: bool):

    class_name = path.rsplit("/", 1)[1].replace(".vcxproj", "")
    category = get_item_regex(path, "FrameLib_Max_Objects\\\\(.*)\\\\fl")
    guid = get_item_regex(path, "<ProjectGuid>\{(.*)\}</ProjectGuid>")

    max_object_path = get_framelib_dir() + "FrameLib_Max_Objects/" + category + "/" + class_name + ".cpp"
    object_class = get_item_regex(max_object_path, "FrameLib_MaxClass_Expand<(.*)>").split(",")[0]

    if object_class == "":
        object_class = get_item_regex(max_object_path, "FrameLib_MaxClass<(.*)>")

    if object_class == "":
        object_class = get_item_regex(max_object_path, "FrameLib_MaxClass_ExprParsed<(.*)>")
    
    if object_class != "":
        vs_object_project_add_rm(object_class, class_name, category, guid, add)
        
    update_vs_solution(class_name, guid, add)


def add_rm_all_vs_projects(add: bool):

    projects = Path(get_vs_project_dir() + "03_Max_Objects/").glob('fl.*.vcxproj')
    for project in projects:
        add_rm_vs_project(project.as_posix(), add)
        
    
def main():

    category = "Test_Category"
    object_class = "FrameLib_Object"
    class_name = "fl.testclass~"
    guid = create_guid()
    
    base_path = get_framelib_dir()
    object_dir = base_path + "FrameLib_Objects/" + category + "/"
    max_object_dir = base_path + "FrameLib_Max_Objects/" + category + "/"
    
    os.makedirs(object_dir, exist_ok = True)
    os.makedirs(max_object_dir, exist_ok = True)
    
    file_create(get_template("fl.class_name~.cpp"), max_object_dir + class_name + ".cpp", object_class, class_name, category, guid)
    file_create(get_template("FrameLib_Class.h"), object_dir + object_class + ".h", object_class, class_name, category, guid)
    file_create(get_template("FrameLib_Class.cpp"), object_dir + object_class + ".cpp", object_class, class_name, category, guid)
    
    max_cpp = single_build_cpp("FrameLib_MaxClass_Expand", object_class, class_name)
    pd_cpp = single_build_cpp("FrameLib_PDClass_Expand", object_class, class_name)
    
    insert_cpp_single_build(max_cpp, category, base_path + "FrameLib_Max_Objects/Common/framelib_max.cpp", "main(", "}")
    insert_cpp_single_build(pd_cpp, category, base_path + "FrameLib_PD_Objects/framelib_pd.cpp", "framelib_pd_setup(", "}")
    insert_object_list_include(object_class, category)

    make_vs_project(object_class, class_name, category, guid)
    
    vs_object_project_add_rm(object_class, class_name, category, guid, True)
    update_vs_solution(class_name, guid, True)
    
    add_xcode_target(object_class, class_name, category)
    
    #add_rm_vs_project(get_vs_project_path(class_name), False)
    #add_rm_all_vs_projects(False)
    
    
if __name__ == "__main__":
    main()
