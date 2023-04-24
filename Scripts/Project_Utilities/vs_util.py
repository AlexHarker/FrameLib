
from . import file_util
from . path_util import fl_paths
from . object_info import fl_object

    
class fl_solution:

    def __init__(self):
    
        with open(fl_paths().vs_solution(), "r") as f:
            self.solution = f.read()
            
        with open(fl_paths().vs_objects_project(), "r") as f:
            self.project = f.read()


    def solution_modify(self, object_info: fl_object, template: str, bounds: list, add: bool):
    
        contents = file_util.templated_string(fl_paths().template("vs_templates/" + template), object_info)
        self.solution = file_util.modify_string(self.solution, contents, bounds, add)
        
        
    def object_project_modify(self, object_info: fl_object, template: str, bounds: list, add: bool):
    
        contents = file_util.templated_string(fl_paths().template("vs_templates/" + template), object_info)
        self.project = file_util.modify_string(self.project, contents, bounds, add)

        
    def update(self, object_info: fl_object, add: bool):

        if object_info.object_class != "":
        
            if fl_paths().object_header_exists(object_info):
                self.object_project_modify(object_info, "header", ["<ClInclude Include", "  </ItemGroup>"], add)
        
            if fl_paths().object_source_exists(object_info):
                self.object_project_modify(object_info, "source", ["<ClCompile Include", "  </ItemGroup>"], add)
        
        self.solution_modify(object_info, "project", ["MinimumVisualStudioVersion", "Global"], add)
        self.solution_modify(object_info, "configurations", ["GlobalSection(ProjectConfigurationPlatforms)", "\tEndGlobalSection"], add)
        self.solution_modify(object_info, "nested", ["GlobalSection(NestedProjects)", "\tEndGlobalSection"], add)
        self.solution_modify(object_info, "dependency", ["\"framelib_objects_max\"", "\tEndProjectSection"], add)
        
        with open(fl_paths().vs_solution(), "w") as f:
            f.write(self.solution)
            
        with open(fl_paths().vs_objects_project(), "w") as f:
            f.write(self.project)
        
        
    def update_project(self, object_info: fl_object):
    
        file_util.create(fl_paths().vs_max_project(object_info), fl_paths().template("fl.class_name~.vcxproj"), object_info)
        
        if object_info.xcode_obj_file_ibuffer_guid != "":
        
            contents = file_util.templated_string(fl_paths().template("vs_templates/ibuffer"), object_info)
            file_util.insert(fl_paths().vs_max_project(object_info), contents, ["<ClCompile Include", "  </ItemGroup>"])
        
