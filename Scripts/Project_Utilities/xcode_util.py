
from . import file_util
from .path_util import fl_paths

import os
import uuid
from pathlib import Path

def relation(guid: str, guid2: str):
        return "\t\t{" + guid + "} = {" + guid2 + "}\n"

def get_guid_regex(exp: str):
    return file_util.item_regex(fl_paths("").vs_solution(), exp)


def get_main_guid():
    return get_guid_regex("Project\(\"\{(.*)\}\"\) = \"framelib\"")


def get_guid(item: str):
    return get_guid_regex("\"" + item + "\".*\{(.*)\}")
    
    
class fl_solution:

    def create_guid(self):
        guid = uuid.uuid4()
        guid = str(guid).upper()
        
        # Ensure that the GUID is not in use already
        
        if file_util.item_regex(fl_paths("").vs_solution(), "(" + guid + ")") != "":
            return self.create_guid()
        
        return guid
        

    def insert_remove(self, path:str, contents: str, start: str, end: str, insert: bool):
        if insert:
            file_util.insert(path, contents, start, end)
        else:
            file_util.remove(path, contents, start, end)


    def solution_insert_remove(self, contents: str, start: str, end: str, insert: bool):
        self.insert_remove(fl_paths("").vs_solution(), contents, start, end, insert)
        
        
    def object_project_insert_remove(self, contents: str, start: str, end: str, insert: bool):
        self.insert_remove(fl_paths("").vs_objects_project(), contents, start, end, insert)


    def solution_add_rm_project(self, class_name: str, guid: str, fl_guid: str, fl_objects_guid: str, insert: bool):

        contents = "Project(\"{" + get_main_guid() + "}\") = \"" + class_name + "\", \"" + "Projects\Visual_Studio\\03_Max_Objects\\" + class_name + ".vcxproj\", \"{" + guid + "}\"\n\tProjectSection(ProjectDependencies) = postProject\n\t\t{" + fl_guid + "} = {" + fl_guid + "}\n\t\t{" + fl_objects_guid + "} = {" + fl_objects_guid + "}\n\tEndProjectSection\nEndProject\n"
            
        self.solution_insert_remove(contents, "MinimumVisualStudioVersion", "Global", insert)
      

    def solution_add_rm_configurations(self, guid: str, insert: bool):

        contents = "\t\t{GUID}.Debug|x64.ActiveCfg = Debug|x64\n\t\t{GUID}.Debug|x64.Build.0 = Debug|x64\n\t\t{GUID}.Debug|x86.ActiveCfg = Debug|x64\n\t\t{GUID}.Release|x64.ActiveCfg = Release|x64\n\t\t{GUID}.Release|x64.Build.0 = Release|x64\n\t\t{GUID}.Release|x86.ActiveCfg = Release|x64\n\t\t{GUID}.Static|x64.ActiveCfg = Release|x64\n\t\t{GUID}.Static|x64.Build.0 = Release|x64\n\t\t{GUID}.Static|x86.ActiveCfg = Release|x64\n\t\t{GUID}.Static|x86.Build.0 = Release|x64\n"
            
        contents = contents.replace("GUID", guid)
        
        self.solution_insert_remove(contents, "GlobalSection(ProjectConfigurationPlatforms) = postSolution", "\tEndGlobalSection", insert)


    def solution_add_rm_nesting(self, guid: str, fl_max_objects_guid: str, insert: bool):

        contents = relation(guid, fl_max_objects_guid)
        self.solution_insert_remove(contents, "GlobalSection(NestedProjects) = preSolution", "\tEndGlobalSection", insert)


    def solution_add_rm_dependency(self, guid: str, insert: bool):

        contents = relation(guid, guid)
        self.solution_insert_remove(contents, "\"framelib_objects_max\"", "\tEndProjectSection", insert)


    def object_project_add_rm(self, object_class: str, class_name: str, category: str, guid: str, insert: bool):

        paths = fl_paths(category)
        
        if os.path.exists(paths.object_header(object_class)):
            header = "    <ClInclude Include=\"..\..\..\FrameLib_Objects\\" + category + "\\" + object_class + ".h\" />\n"
            self.object_project_insert_remove(header, "<ClInclude Include", "  </ItemGroup>", insert)
        
        if os.path.exists(paths.object_source(object_class)):
            cplusplus = "    <ClCompile Include=\"..\..\..\FrameLib_Objects\\" + category + "\\" + object_class + ".cpp\" />\n"
            self.object_project_insert_remove(cplusplus, "<ClCompile Include", "  </ItemGroup>", insert)

        
    def update(self, object_class: str, class_name: str, category: str, guid: str, add: bool):

        fl_guid = get_guid("framelib")
        fl_objects_guid = get_guid("framelib_objects")
        fl_max_objects_guid = get_guid("Max Object Projects")
        
        if object_class != "":
            self.object_project_add_rm(object_class, class_name, category, guid, add)
        
        self.solution_add_rm_project(class_name, guid, fl_guid, fl_objects_guid, add)
        self.solution_add_rm_configurations(guid, add)
        self.solution_add_rm_nesting(guid, fl_max_objects_guid, add)
        self.solution_add_rm_dependency(guid, add)
        
        
    def update_project(self, path: str, add: bool):

        class_name = path.rsplit("/", 1)[1].replace(".vcxproj", "")
        category = file_util.item_regex(path, "FrameLib_Max_Objects\\\\(.*)\\\\fl")
        guid = file_util.item_regex(path, "<ProjectGuid>\{(.*)\}</ProjectGuid>")

        paths = fl_paths(category)
        max_object_path = paths.max_source(class_name)
        object_class = file_util.item_regex(max_object_path, "FrameLib_MaxClass_Expand<(.*)>").split(",")[0]

        if object_class == "":
            object_class = file_util.item_regex(max_object_path, "FrameLib_MaxClass<(.*)>")

        if object_class == "":
            object_class = file_util.item_regex(max_object_path, "FrameLib_MaxClass_ExprParsed<(.*)>")
   
        self.update(object_class, class_name, category, guid, add)
        
        file_util.create(paths.template("fl.class_name~.vcxproj"), paths.vs_max_project(class_name), object_class, class_name, category, guid)


    def update_all_projects(self, add: bool):

        projects = Path(fl_paths("").vs_max_projects()).glob('fl.*.vcxproj')
        project_list = list(projects)
        project_list.sort()
        
        for project in project_list:
            print(project)
            self.update_project(project.as_posix(), add)
