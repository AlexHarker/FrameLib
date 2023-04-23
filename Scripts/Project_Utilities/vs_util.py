
from . import file_util
from . path_util import fl_paths
from . object_info import fl_object

    
class fl_solution:

    def solution_modify(self, object_info: fl_object, template: str, bounds: list, add: bool):
    
        contents = file_util.templated_string(fl_paths().template("vs_templates/" + template), object_info)
        file_util.modify(fl_paths().vs_solution(), contents, bounds, add)
        
        
    def object_project_modify(self, object_info: fl_object, template: str, bounds: list, add: bool):
    
        contents = file_util.templated_string(fl_paths().template("vs_templates/" + template), object_info)
        file_util.modify(fl_paths().vs_objects_project(), contents, bounds, add)

        
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
        
        
    def update_project(self, object_info: fl_object, add: bool):
        file_util.create(fl_paths().vs_max_project(object_info), fl_paths().template("fl.class_name~.vcxproj"), object_info)
