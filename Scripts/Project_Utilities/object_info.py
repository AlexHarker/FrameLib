
import json

class fl_object:
    
    object_cache = {}
    static_guids = {}
    
    initialised = False
    
    def __init__(self, object_class: str, class_name: str, category: str, source_type: str = ""):
        
        from . path_util import fl_paths
        from . file_util import do_regex
        from . file_util import item_regex
        from . guid_util import guid_manager
        from . xc_util import section_bounds

        import copy
        import os
        
        self.object_class = object_class
        self.object_class_name_upper = object_class.upper()
        self.object_class_file = do_regex(object_class, "([^:]*)")
        self.max_class_name = class_name
        self.pd_class_name = class_name
        self.category = category
        
        project_path = fl_paths().vs_max_project(self)
        project_exists = os.path.exists(project_path)
        guids = guid_manager()
        
        if fl_object.initialised == False:
                    
            static_guids = {}
            
            static_guids["vs_framelib_guid"] = guids.vs("framelib")
            static_guids["vs_framelib_obj_guid"] = guids.vs("framelib_objects")
            static_guids["vs_max_objects_guid"] = guids.vs("Max Object Projects")
            static_guids["vs_main_guid"] = guids.vs_main()
                            
            static_guids["xc_main_guid"] = guids.xc("PBXProject", "Project object")
            static_guids["xc_framelib_guid"] = guids.xc("PBXNativeTarget", "framelib")
            static_guids["xc_max_config_guid"] = guids.xc("PBXFileReference", "Config_FrameLib_Max.xcconfig")
            static_guids["xc_fileref_lib_guid"] = guids.xc("PBXFileReference", "libframelib.a")
            static_guids["xc_fileref_fft_guid"] = guids.xc("PBXFileReference", "HISSTools_FFT.cpp")
            static_guids["xc_fileref_ibuffer_guid"] = guids.xc("PBXFileReference", "ibuffer_access.cpp")
    
            static_guids["xc_lib_sources_guid"] = guids.xc_component("PBXNativeTarget", "framelib_objects", "buildPhases", "Sources")
            
            fl_object.static_guids = static_guids
            fl_object.initialised = True

        if class_name in fl_object.object_cache:
        
            self.__dict__ = copy.deepcopy(fl_object.object_cache[class_name])
            return
            
        elif project_exists:
        
            import re

            self.category = item_regex(project_path, "FrameLib_Max_Objects\\\\(.*)\\\\fl")

            max_object_path = fl_paths().max_source(self)
            self.object_class = item_regex(max_object_path, "FrameLib_MaxClass_Expand<(.*)>").split(",")[0]

            if self.object_class == "":
                self.object_class = item_regex(max_object_path, "FrameLib_MaxClass<(.*)>")

            if self.object_class == "":
                self.object_class = item_regex(max_object_path, "FrameLib_MaxClass_ExprParsed<(.*)>")

            self.object_class_name_upper = self.object_class.upper()
            self.object_class_file = do_regex(self.object_class, "([^:]*)")
            
            self.vs_project_guid = item_regex(project_path, "<ProjectGuid>\{(.*)\}</ProjectGuid>")

            self.xc_obj_target_guid = guids.xc("PBXNativeTarget", self.max_class_name)

            bounds = section_bounds("PBXTargetDependency") + ["/* Begin PBXTargetDependency", "target = " + self.xc_obj_target_guid]
            exp = "[\S\s]*\s([^\s]+) /\* PBXTargetDependency \*/ = \{[\S\s]*?\Z"
            self.xc_obj_package_dep_guid = guids.xc_custom(bounds, exp)
            
            self.xc_obj_lib_dep_guid = guids.xc_component("PBXNativeTarget", self.max_class_name, "dependencies", "PBXTargetDependency")

            self.xc_obj_lib_proxy_guid = guids.xc_field("PBXTargetDependency", self.xc_obj_lib_dep_guid, "targetProxy")
            self.xc_obj_target_proxy_guid = guids.xc_field("PBXTargetDependency", self.xc_obj_package_dep_guid, "targetProxy")
            
            self.xc_obj_sources_guid = guids.xc_component("PBXNativeTarget", self.max_class_name, "buildPhases", "Sources")
            self.xc_obj_frameworks_guid = guids.xc_component("PBXNativeTarget", self.max_class_name, "buildPhases", "Frameworks")

            class_str = self.max_class_name + ".cpp in Sources"
            object_str = self.object_class_file + ".cpp in Sources"
            fft_str = "HISSTools_FFT.cpp in Sources"
            ibuffer_str = "ibuffer_access.cpp in Sources"
            lib_str = "libframelib.a in Frameworks"
            
            sources_guid = self.xc_obj_sources_guid
            frameworks_guid = self.xc_obj_frameworks_guid
            lib_guid = fl_object.static_guids["xc_lib_sources_guid"]

            self.xc_obj_file_class_guid = guids.xc_component("PBXSourcesBuildPhase", "Sources", "files", class_str, sources_guid)
            self.xc_obj_file_object_guid = guids.xc_component("PBXSourcesBuildPhase", "Sources", "files", object_str, sources_guid)
            self.xc_obj_file_lib_guid = guids.xc_component("PBXFrameworksBuildPhase", "Frameworks", "files", lib_str, frameworks_guid)
            self.xc_obj_file_object_for_lib_guid = guids.xc_component("PBXSourcesBuildPhase", "Sources", "files", object_str, lib_guid)
            self.xc_obj_file_fft_guid = guids.xc_component("PBXSourcesBuildPhase", "Sources", "files", fft_str, sources_guid)
            self.xc_obj_file_ibuffer_guid = guids.xc_component("PBXSourcesBuildPhase", "Sources", "files", ibuffer_str, sources_guid)

            self.xc_obj_fileref_class_guid = guids.xc("PBXFileReference", self.max_class_name + ".cpp")
            self.xc_obj_fileref_object_guid = guids.xc("PBXFileReference", self.object_class_file + ".cpp")
            self.xc_obj_fileref_header_guid = guids.xc("PBXFileReference", self.object_class_file + ".h")
            self.xc_obj_fileref_mxo_guid = guids.xc("PBXFileReference", self.max_class_name + ".mxo")
            
            config_list = "Build configuration list for PBXNativeTarget \"" + self.max_class_name + "\""
            
            self.xc_obj_config_list_guid = guids.xc("XCConfigurationList", config_list)
            self.xc_obj_config_dvmt_guid = guids.xc_component("XCConfigurationList", config_list, "buildConfigurations", "Development")
            self.xc_obj_config_dplt_guid = guids.xc_component("XCConfigurationList", config_list, "buildConfigurations", "Deployment")
            self.xc_obj_config_test_guid = guids.xc_component("XCConfigurationList", config_list, "buildConfigurations", "Public Testing")
            
        else:
        
            self.vs_project_guid = guids.create_vs()
            
            self.xc_obj_target_guid = guids.create_xc()
            self.xc_obj_package_dep_guid = guids.create_xc()
            self.xc_obj_lib_dep_guid = guids.create_xc()

            self.xc_obj_lib_proxy_guid = guids.create_xc()
            self.xc_obj_target_proxy_guid = guids.create_xc()
            
            self.xc_obj_sources_guid = guids.create_xc()
            self.xc_obj_frameworks_guid = guids.create_xc()

            self.xc_obj_file_class_guid = guids.create_xc()
            self.xc_obj_file_object_guid = guids.create_xc()
            self.xc_obj_file_lib_guid = guids.create_xc()
            self.xc_obj_file_object_for_lib_guid = guids.create_xc()
            
            if source_type == "fft":
                self.xc_obj_file_fft_guid = guids.create_xc()
                self.xc_obj_file_ibuffer_guid = ""
            elif source_type == "ibuffer":
                self.xc_obj_file_fft_guid = ""
                self.xc_obj_file_ibuffer_guid = guids.create_xc()
            else:
                self.xc_obj_file_fft_guid = ""
                self.xc_obj_file_ibuffer_guid = ""
                
            self.xc_obj_fileref_class_guid = guids.create_xc()
            self.xc_obj_fileref_object_guid = guids.create_xc()
            self.xc_obj_fileref_header_guid = guids.create_xc()
            self.xc_obj_fileref_mxo_guid = guids.create_xc()
            
            self.xc_obj_config_list_guid = guids.create_xc()
            self.xc_obj_config_dvmt_guid = guids.create_xc()
            self.xc_obj_config_dplt_guid = guids.create_xc()
            self.xc_obj_config_test_guid = guids.create_xc()
        
        fl_object.object_cache[class_name] = self.__dict__
        
    
    def object_file_external(self):
        return self.object_class != self.object_class_file
    
    
    @staticmethod
    def create_from_name(class_name: str):
        return fl_object("", class_name, "")
        
        
    @staticmethod
    def save_cache():

        from . path_util import fl_paths

        with open(fl_paths().cache_path(), "w+") as f:
            json.dump(fl_object.object_cache, f, indent=4)
        
        
    @staticmethod
    def load_cache():
    
        from . path_util import fl_paths
        import os

        if os.path.exists(fl_paths().cache_path()):
            with open(fl_paths().cache_path(), "r") as f:
                fl_object.object_cache = json.load(f)

