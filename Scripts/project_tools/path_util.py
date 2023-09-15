
from . object_info import fl_object

from pathlib import Path


class fl_paths:

    def __init__(self):

        file_path = Path(__file__)
        self.base_dir = file_path.absolute().parent.parent.parent.as_posix() + "/"
        self.vs_project_dir = self.base_dir + "Projects/Visual_Studio/"
        self.template_dir = self.base_dir + "Scripts/project_tools/base_templates/"
        self.max_dir = self.base_dir + "FrameLib_Max_Objects/"

    def vs_template(self, file_name: str):
        return self.template_dir + "vs_templates/" + file_name
        
    def xc_template(self, file_name: str):
        return self.template_dir + "xc_templates/" +  file_name
        
    def code_template(self, file_name: str):
        return self.template_dir + "code_templates/" +  file_name
        
    def max_object_dir(self, info: fl_object):
        return self.max_dir + info["category"] + "/"
    
    def object_dir(self, info: fl_object):
        return self.base_dir + "FrameLib_Objects/" + info["category"] + "/"
        
    def max_source(self, info: fl_object):
        return self.max_object_dir(info) + info["max_class_name"] + ".cpp"
    
    def object_source(self, info: fl_object):
        return self.object_dir(info) + info["object_class_file"] + ".cpp"

    def object_source_exists(self, info: fl_object):
        return Path(self.object_source(info)).exists()
        
    def object_header(self, info: fl_object):
        return self.object_dir(info) + info["object_class_file"] + ".h"
        
    def object_header_exists(self, info: fl_object):
        return Path(self.object_header(info)).exists()
    
    def xc_project_dir(self):
        return self.base_dir + "framelib.xcodeproj/"
        
    def xc_pbxproj(self):
        return self.xc_project_dir() + "project.pbxproj"
        
    def xc_scheme(self):
        return self.xc_project_dir() + "xcshareddata/xcschemes/framelib Max (objects build).xcscheme"
        
    def vs_solution(self):
        return self.base_dir + "framelib.sln"
    
    def vs_objects_project(self):
        return self.vs_project_dir + "02_Common/framelib_objects.vcxproj"
        
    def vs_max_projects(self):
        return self.vs_project_dir + "03_Max_Objects/"
        
    def vs_max_project(self, info: fl_object):
        return self.vs_max_projects() + info["max_class_name"] + ".vcxproj"

    def objects_export_header(self):
        return self.base_dir + "FrameLib_Exports/FrameLib_Objects.h"
        
    def pd_framelib(self):
        return self.base_dir + "FrameLib_PD_Objects/framelib_pd.cpp"
    
    def max_framelib(self):
        return self.max_dir + "Common/framelib_max.cpp"
        
    def cache_path(self):
        return self.template_dir + "cache/object_cache.json"
        
