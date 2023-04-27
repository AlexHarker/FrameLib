
from . path_util import fl_paths
from . object_info import fl_object

from . import file_util


# Helper for global section bounds

def bounds_global(name: str):
    return ["GlobalSection(" + name + ")", "\tEndGlobalSection"]
    

# Helper for included file bounds

def bounds_include():
    return ["    <ClInclude Include", "  </ItemGroup>"]
    

# Helper for compiled file bounds

def bounds_compiled():
    return ["    <ClCompile Include", "  </ItemGroup>"]
    
    
class fl_solution:

    def __init__(self):
    
        self.solution = file_util.rw_file(fl_paths().vs_solution())
        self.project = file_util.rw_file(fl_paths().vs_objects_project())


    def solution_modify(self, info: fl_object, template: str, bounds: list, add: bool):
    
        # Add or remove templated contents from the VS solution
        
        contents = file_util.templated_string(fl_paths().vs_template(template), info)
        self.solution.data = file_util.modify(self.solution.data, contents, bounds, add)
        
        
    def object_project_modify(self, info: fl_object, template: str, bounds: list, add: bool):
    
        # Add or remove templated contents from the max object VS project
            
        contents = file_util.templated_string(fl_paths().vs_template(template), info)
        self.project.data = file_util.modify(self.project.data, contents, bounds, add)

        
    def update(self, info: fl_object, add: bool):

        # Modify the frmaleib objects project to include/remove files for the framelib object
        
        if info["object_class"] != "":
        
            if fl_paths().object_header_exists(info) and not info.object_file_external():
                self.object_project_modify(info, "header", ["    <ClInclude Include", "  </ItemGroup>"], add)
        
            if fl_paths().object_source_exists(info) and not info.object_file_external():
                self.object_project_modify(info, "source", ["    <ClCompile Include", "  </ItemGroup>"], add)
        
        # Modify the relevant sections of the solution to add/remove the max object project
        
        self.solution_modify(info, "project", ["MinimumVisualStudioVersion", "Global"], add)
        self.solution_modify(info, "configurations", bounds_global("ProjectConfigurationPlatforms"), add)
        self.solution_modify(info, "nested", bounds_global("NestedProjects"), add)
        self.solution_modify(info, "dependency", ["\"framelib_objects_max\"", "\tEndProjectSection"], add)
        
        # Flush to disk
        
        self.solution.flush()
        self.project.flush()
        
        
    def update_project(self, info: fl_object, overwrite: bool = False):
    
        # Create a mx object VS project
        
        project_path = fl_paths().vs_max_project(info)
        template_path = fl_paths().vs_template("_class_name~.vcxproj")

        file_util.create(project_path, template_path, info, overwrite)
        
        # Add the ibuffer source to the project if required
        
        if info["xc_obj_file_ibuffer_guid"] != "":

            contents = file_util.templated_string(fl_paths().vs_template("ibuffer"), info)
            file_util.insert_file(fl_paths().vs_max_project(info), contents, bounds_compiled())
        

    def sort_project(self):
    
        # Sort the framelib objects project sources and includes into lexicographical order
        
        exp = "\"(.*)\""
        self.project.data = file_util.sort_section(self.project.data, bounds_include(), exp)
        self.project.data = file_util.sort_section(self.project.data, bounds_compiled(), exp)
   
        self.project.flush()
