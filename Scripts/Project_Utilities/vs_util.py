
from . import file_util
from . path_util import fl_paths
from . object_info import fl_object

import os
import uuid
from pathlib import Path

    
class fl_solution:

    def create_guid(self):
        guid = uuid.uuid4()
        guid = str(guid).upper()
        
        # Ensure that the GUID is not in use already
        
        if file_util.item_regex(fl_paths().vs_solution(), "(" + guid + ")") != "":
            return self.create_guid()
        
        return guid
        

    def insert_remove(self, path:str, contents: str, start: str, end: str, insert: bool):
        if insert:
            file_util.insert(path, contents, start, end)
        else:
            file_util.remove(path, contents, start, end)


    def solution_modify(self, object_info: fl_object, template: str, start: str, end: str, insert: bool):
    
        contents = file_util.templated_string(fl_paths().template("vs_templates/" + template), object_info)
        self.insert_remove(fl_paths().vs_solution(), contents, start, end, insert)
        
        
    def object_project_modify(self, object_info: fl_object, template: str, start: str, end: str, insert: bool):
    
        contents = file_util.templated_string(fl_paths().template("vs_templates/" + template), object_info)
        self.insert_remove(fl_paths().vs_objects_project(), contents, start, end, insert)

        
    def update(self, object_info: fl_object, add: bool):

        if object_info.object_class != "":
        
            if os.path.exists(fl_paths().object_header(object_info)):
                self.object_project_modify(object_info, "header", "<ClInclude Include", "  </ItemGroup>", add)
        
            if os.path.exists(fl_paths().object_source(object_info)):
                self.object_project_modify(object_info, "source", "<ClCompile Include", "  </ItemGroup>", add)
        
        self.solution_modify(object_info, "project", "MinimumVisualStudioVersion", "Global", add)
        self.solution_modify(object_info, "configurations", "GlobalSection(ProjectConfigurationPlatforms)", "\tEndGlobalSection", add)
        self.solution_modify(object_info, "nested", "GlobalSection(NestedProjects)", "\tEndGlobalSection", add)
        self.solution_modify(object_info, "dependency", "\"framelib_objects_max\"", "\tEndProjectSection", add)
        
        
    def update_project(self, path: str, add: bool):

        class_name = path.rsplit("/", 1)[1].replace(".vcxproj", "")
        category = file_util.item_regex(path, "FrameLib_Max_Objects\\\\(.*)\\\\fl")
        guid = file_util.item_regex(path, "<ProjectGuid>\{(.*)\}</ProjectGuid>")

        object_info = fl_object("", class_name, category)
        object_info.guid = guid

        paths = fl_paths()
        max_object_path = paths.max_source(object_info)
        object_info.object_class = file_util.item_regex(max_object_path, "FrameLib_MaxClass_Expand<(.*)>").split(",")[0]

        if object_info.object_class == "":
            object_info.object_class = file_util.item_regex(max_object_path, "FrameLib_MaxClass<(.*)>")

        if object_info.object_class == "":
            object_info.object_class = file_util.item_regex(max_object_path, "FrameLib_MaxClass_ExprParsed<(.*)>")
           
        self.update(object_info, add)
        
        file_util.create(paths.vs_max_project(object_info), paths.template("fl.class_name~.vcxproj"), object_info)


    def update_all_projects(self, add: bool):

        projects = Path(fl_paths().vs_max_projects()).glob('fl.*.vcxproj')
        project_list = list(projects)
        project_list.sort()
        
        for project in project_list:
            print(project)
            self.update_project(project.as_posix(), add)
