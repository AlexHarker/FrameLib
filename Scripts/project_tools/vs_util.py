
from . import file_util
from . path_util import fl_paths
from . object_info import fl_object

    
class fl_solution:

    def __init__(self):
    
        self.solution = file_util.rw_file(fl_paths().vs_solution())
        self.project = file_util.rw_file(fl_paths().vs_objects_project())


    def solution_modify(self, object_info: fl_object, template: str, bounds: list, add: bool):
    
        contents = file_util.templated_string(fl_paths().vs_template(template), object_info)
        self.solution.data = file_util.modify(self.solution.data, contents, bounds, add)
        
        
    def object_project_modify(self, object_info: fl_object, template: str, bounds: list, add: bool):
    
        contents = file_util.templated_string(fl_paths().vs_template(template), object_info)
        self.project.data = file_util.modify(self.project.data, contents, bounds, add)

        
    def update(self, object_info: fl_object, add: bool):

        if object_info["object_class"] != "":
        
            if fl_paths().object_header_exists(object_info) and not object_info.object_file_external():
                self.object_project_modify(object_info, "header", ["<ClInclude Include", "  </ItemGroup>"], add)
        
            if fl_paths().object_source_exists(object_info) and not object_info.object_file_external():
                self.object_project_modify(object_info, "source", ["<ClCompile Include", "  </ItemGroup>"], add)
        
        self.solution_modify(object_info, "project", ["MinimumVisualStudioVersion", "Global"], add)
        self.solution_modify(object_info, "configurations", ["GlobalSection(ProjectConfigurationPlatforms)", "\tEndGlobalSection"], add)
        self.solution_modify(object_info, "nested", ["GlobalSection(NestedProjects)", "\tEndGlobalSection"], add)
        self.solution_modify(object_info, "dependency", ["\"framelib_objects_max\"", "\tEndProjectSection"], add)
        
        self.solution.flush()
        self.project.flush()
        
        
    def update_project(self, object_info: fl_object, overwrite: bool = False):
    
        file_util.create(fl_paths().vs_max_project(object_info), fl_paths().vs_template("_class_name~.vcxproj"), object_info, overwrite)
        
        if object_info["xc_obj_file_ibuffer_guid"] != "":

            contents = file_util.templated_string(fl_paths().vs_template("ibuffer"), object_info)
            file_util.insert_file(fl_paths().vs_max_project(object_info), contents, ["<ClCompile Include", "  </ItemGroup>"])
        

    def sort_project(self):
    
        exp = "\"(.*)\""
        self.project.data = file_util.sort_section(self.project.data, ["<ClInclude Include", "  </ItemGroup>"], exp)
        self.project.data = file_util.sort_section(self.project.data, ["<ClCompile Include", "  </ItemGroup>"], exp)
   
        self.project.flush()
