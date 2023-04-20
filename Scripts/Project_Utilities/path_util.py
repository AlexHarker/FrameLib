import os

class fl_paths:

    def __init__(self, category: str):
        self.base_dir = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))) + "/"
        self.vs_project_dir = self.base_dir + "Projects/Visual_Studio/"
        self.template_dir = self.base_dir + "Scripts/Resources/"
        self.object_dir = self.base_dir + "FrameLib_Objects/" + category + "/"
        self.max_dir = self.base_dir + "FrameLib_Max_Objects/"
        self.max_object_dir = self.max_dir + category + "/"
        
    def template(self, file_name: str):
        return self.template_dir + file_name
        
    def max_source(self, class_name: str):
        return self.max_object_dir + class_name + ".cpp"
    
    def object_source(self, object_class: str):
        return self.object_dir + object_class + ".cpp"

    def object_header(self, object_class: str):
        return self.object_dir + object_class + ".h"
        
    def vs_solution(self):
        return self.base_dir + "framelib.sln"
    
    def vs_objects_project(self):
        return self.vs_project_dir + "02_Common/framelib_objects.vcxproj"
        
    def vs_max_projects(self):
        return self.vs_project_dir + "03_Max_Objects/"
        
    def vs_max_project(self, class_name: str):
        return self.vs_max_projects() + class_name + ".vcxproj"

    def objects_export_header(self):
        return self.base_dir + "FrameLib_Exports/FrameLib_Objects.h"
        
    def pd_framelib(self):
        return self.base_dir + "FrameLib_PD_Objects/framelib_pd.cpp"
    
    def max_framelib(self):
        return self.max_dir + "Common/framelib_max.cpp"

