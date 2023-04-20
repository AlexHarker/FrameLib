import os
import uuid
import re
from pathlib import Path


def get_framelib_dir():
    return os.path.dirname(os.path.dirname(os.path.abspath(__file__))) + "/"
    
    
def get_vs_project_dir():
    return get_framelib_dir() + "Projects/Visual_Studio/"
    
    
def get_vs_project_path(class_name: str):
    return get_vs_project_dir() + "03_Max_Objects/" + "fl." + class_name + "~.vcxproj"


def get_template_dir():
    return get_framelib_dir() + "Scripts/Resources/"
    
    
def get_template(file_name: str):
    return get_template_dir() + file_name


def create_guid():
    guid = uuid.uuid4()
    return str(guid).upper()
    
    
def get_guid_regex(exp: str):
    regex = re.compile(exp)
    
    with open(get_framelib_dir() + "framelib.sln") as f:
        data = f.read()
        match = regex.search(data)
        
        if match is not None:
            return match.group(1)

    return ""


def get_main_guid():
    return get_guid_regex("Project\(\"\{(.*)\}\"\) = \"framelib\"")


def get_guid(item: str):
    return get_guid_regex("\"" + item + "\".*\{(.*)\}")
    
    
def add_xcode_target(object_class: str, class_name: str, category: str):
    #add target
    #setup coorectly for path
    #change/set source files
    #add target as a dependecy for the max objects build
    return ""


def file_create(template_path: str, output_path: str, object_class: str, class_name: str, category: str, guid: str):

    f = open(template_path, "r")
    template = f.read()
    f.close()

    template = template.replace("##CLASS##", object_class)
    template = template.replace("##CLASS_UPPER##", object_class.upper())
    template = template.replace("##CLASSNAME##", class_name)
    template = template.replace("##CATEGORY##", category)
    template = template.replace("##GUID##", guid)
    
    f = open(output_path, "x")
    f.write(template)
    f.close()


def make_vs_project(object_class: str, class_name: str, category: str, guid: str):

    file_create(get_template("fl.class_name~.vcxproj"), get_vs_project_path(class_name), object_class, class_name, category, guid)


def vs_file_insert(path: str, contents: str, start: str, end: str):
    
    data = ""
    end = end + "\n"
    
    with open(path, "r") as f:
        data = f.read()
        index = data.find(start) + len(start)
        index = data.find(end, index)
        
    with open(path, "w") as f:
        f.write(data[:index] + contents + data[index:])


def vs_solution_insert(contents: str, start: str, end: str):
    vs_file_insert(get_framelib_dir() + "framelib.sln", contents, start, end)


def vs_object_project_insert(contents: str, start: str, end: str):
    vs_file_insert(get_vs_project_dir() + "02_Common/framelib_objects.vcxproj", contents, start, end)
    

def vs_solution_add_project(class_name: str, guid: str, fl_guid: str, fl_objects_guid: str):

    contents = "Project(\"{" + get_main_guid() + "}\") = \"" + class_name + "\", \"" + get_vs_project_path(class_name) + "\", \"{" + guid + "}\"\n\tProjectSection(ProjectDependencies) = postProject\n\t\t{" + fl_guid + "} = {" + fl_guid + "}\n\t\t{" + fl_objects_guid + "} = {" + fl_objects_guid + "}\n\tEndProjectSection\nEndProject\n"
        
    vs_solution_insert(contents, "MinimumVisualStudioVersion", "Global")


def vs_solution_add_configurations(guid: str):

    contents = "\t\t{##GUID##}.Debug|x64.ActiveCfg = Debug|x64\n\t\t{##GUID##}.Debug|x64.Build.0 = Debug|x64\n\t\t{##GUID##}.Debug|x86.ActiveCfg = Debug|x64\n\t\t{##GUID##}.Release|x64.ActiveCfg = Release|x64\n\t\t{##GUID##}.Release|x64.Build.0 = Release|x64\n\t\t{##GUID##}.Release|x86.ActiveCfg = Release|x64\n\t\t{##GUID##}.Static|x64.ActiveCfg = Release|x64\n\t\t{##GUID##}.Static|x64.Build.0 = Release|x64\n\t\t{##GUID##}.Static|x86.ActiveCfg = Release|x64\n\t\t{##GUID##}.Static|x86.Build.0 = Release|x64\n"
        
    contents = contents.replace("##GUID##", guid)
    
    vs_solution_insert(contents, "GlobalSection(ProjectConfigurationPlatforms) = postSolution", "\tEndGlobalSection")
   
   
def vs_solution_relation(guid: str, guid2: str):
    return "\t\t{" + guid + "} = {" + guid2 + "}\n"
        
        
def vs_solution_add_nesting(guid: str):

    contents = vs_solution_relation(guid, guid)
    vs_solution_insert(contents, "GlobalSection(NestedProjects) = preSolution", "\tEndGlobalSection")


def vs_solution_add_dependency(guid: str, fl_max_objects_guid: str):

    contents = vs_solution_relation(guid, fl_max_objects_guid)
    vs_solution_insert(contents, "\"framelib_objects_max\"", "\tEndProjectSection")


def update_vs_object_project(object_class: str, class_name: str, category: str, guid: str):

    header = "    <ClInclude Include=\"..\..\..\FrameLib_Objects\\" + category + "\FrameLib_" + object_class + ".h\" />\n"
    cplusplus = "    <ClCompile Include=\"..\..\..\FrameLib_Objects\\" + category + "\FrameLib_" + object_class + ".cpp\" />\n"
    
    vs_object_project_insert(header, "<ClInclude Include", "  </ItemGroup>")
    vs_object_project_insert(cplusplus, "<ClCompile Include", "  </ItemGroup>")
    
    
def update_vs_solution(object_class: str, class_name: str, category: str, guid: str):

    fl_guid = get_guid("framelib")
    fl_objects_guid = get_guid("framelib_objects")
    fl_max_objects_guid = get_guid("Max Object Projects")
    
    vs_solution_add_project(class_name, guid, fl_guid, fl_objects_guid)
    vs_solution_add_configurations(guid)
    vs_solution_add_nesting(guid)
    vs_solution_add_dependency(guid, fl_max_objects_guid)
    
    
def main():

    category = "TestCategory"
    object_class = "Object"
    class_name = "testclass"
    
    guid = create_guid()
    
    base_path = get_framelib_dir()
    object_dir = base_path + "FrameLib_Objects/" + category + "/"
    max_object_dir = base_path + "FrameLib_Max_Objects/" + category + "/"
    
    os.makedirs(object_dir, exist_ok = True)
    os.makedirs(max_object_dir, exist_ok = True)
    
    file_create(get_template("fl.class_name~.cpp"), max_object_dir + "fl." + class_name + "~.cpp", object_class, class_name, category, guid)
    file_create(get_template("FrameLib_Class.h"), object_dir + "FrameLib_" + object_class + ".h", object_class, class_name, category, guid)
    file_create(get_template("FrameLib_Class.cpp"), object_dir + "FrameLib_" + object_class + ".cpp", object_class, class_name, category, guid)
    
    make_vs_project(object_class, class_name, category, guid)
    update_vs_object_project(object_class, class_name, category, guid)
    update_vs_solution(object_class, class_name, category, guid)
    add_xcode_target(object_class, class_name, category)
    
if __name__ == "__main__":
    main()
