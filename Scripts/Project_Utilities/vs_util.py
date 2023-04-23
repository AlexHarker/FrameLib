
from . import file_util
from . path_util import fl_paths
from . object_info import fl_object

import os
from pathlib import Path

    
class fl_solution:

    def solution_modify(self, object_info: fl_object, template: str, bounds: list, add: bool):
    
        contents = file_util.templated_string(fl_paths().template("vs_templates/" + template), object_info)
        file_util.modify(fl_paths().vs_solution(), contents, bounds, add)
        
        
    def object_project_modify(self, object_info: fl_object, template: str, bounds: list, add: bool):
    
        contents = file_util.templated_string(fl_paths().template("vs_templates/" + template), object_info)
        file_util.modify(fl_paths().vs_objects_project(), contents, bounds, add)

        
    def update(self, object_info: fl_object, add: bool):

        if object_info.object_class != "":
        
            if os.path.exists(fl_paths().object_header(object_info)):
                self.object_project_modify(object_info, "header", ["<ClInclude Include", "  </ItemGroup>"], add)
        
            if os.path.exists(fl_paths().object_source(object_info)):
                self.object_project_modify(object_info, "source", ["<ClCompile Include", "  </ItemGroup>"], add)
        
        self.solution_modify(object_info, "project", ["MinimumVisualStudioVersion", "Global"], add)
        self.solution_modify(object_info, "configurations", ["GlobalSection(ProjectConfigurationPlatforms)", "\tEndGlobalSection"], add)
        self.solution_modify(object_info, "nested", ["GlobalSection(NestedProjects)", "\tEndGlobalSection"], add)
        self.solution_modify(object_info, "dependency", ["\"framelib_objects_max\"", "\tEndProjectSection"], add)
        
        
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
