
from . object_info import fl_object

import os

class fl_paths:

    def __init__(self):

        import os

        self.base_dir = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))) + "/"
        self.vs_project_dir = self.base_dir + "Projects/Visual_Studio/"
        self.template_dir = self.base_dir + "Scripts/Resources/"
        self.max_dir = self.base_dir + "FrameLib_Max_Objects/"

    def vs_template(self, file_name: str):
        return self.template_dir + "vs_templates/" + file_name
        
    def xc_template(self, file_name: str):
        return self.template_dir + "xc_templates/" +  file_name
        
    def code_template(self, file_name: str):
        return self.template_dir + "code_templates/" +  file_name
        
    def max_object_dir(self, object_info: fl_object):
        return self.max_dir + object_info["category"] + "/"
    
    def object_dir(self, object_info: fl_object):
        return self.base_dir + "FrameLib_Objects/" + object_info["category"] + "/"
        
    def max_source(self, object_info: fl_object):
        return self.max_object_dir(object_info) + object_info["max_class_name"] + ".cpp"
    
    def object_source(self, object_info: fl_object):
        return self.object_dir(object_info) + object_info["object_class_file"] + ".cpp"

    def object_source_exists(self, object_info: fl_object):
        return os.path.exists(self.object_source(object_info))
        
    def object_header(self, object_info: fl_object):
        return self.object_dir(object_info) + object_info["object_class_file"] + ".h"
        
    def object_header_exists(self, object_info: fl_object):
        return os.path.exists(self.object_header(object_info))
    
    def xc_pbxproj(self):
        return self.base_dir + "framelib.xcodeproj/project.pbxproj"
        
    def xc_scheme(self):
        return self.base_dir + "framelib.xcodeproj/xcshareddata/xcschemes/framelib Max (objects build).xcscheme"
        
    def vs_solution(self):
        return self.base_dir + "framelib.sln"
    
    def vs_objects_project(self):
        return self.vs_project_dir + "02_Common/framelib_objects.vcxproj"
        
    def vs_max_projects(self):
        return self.vs_project_dir + "03_Max_Objects/"
        
    def vs_max_project(self, object_info: fl_object):
        return self.vs_max_projects() + object_info["max_class_name"] + ".vcxproj"

    def objects_export_header(self):
        return self.base_dir + "FrameLib_Exports/FrameLib_Objects.h"
        
    def pd_framelib(self):
        return self.base_dir + "FrameLib_PD_Objects/framelib_pd.cpp"
    
    def max_framelib(self):
        return self.max_dir + "Common/framelib_max.cpp"
        
    def cache_path(self):
        return self.template_dir + "object_cache.json"
        

