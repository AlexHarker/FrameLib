
class fl_object:
    
    object_cache = {}
    
    vs_framelib_guid = ""
    vs_framelib_obj_guid = ""
    vs_max_objects_guid = ""
    vs_main_guid = ""
            
    xcode_main_guid = ""
    xcode_framelib_guid = ""
    xcode_max_config_guid = ""
    xcode_fileref_lib_guid = ""
    xcode_lib_sources_guid = ""
    
    initialised = False
    
    def __init__(self, object_class: str, class_name: str, category: str, source_type: str = ""):
        
        from . path_util import fl_paths
        from . file_util import item_regex
        from . guid_util import guid_manager
        from . xcode_util import section_bounds

        import copy
        import os
        
        self.object_class = object_class
        self.max_class_name = class_name
        self.max_class_name_upper = class_name.upper()
        self.pd_class_name = class_name
        self.category = category
        
        project_path = fl_paths().vs_max_project(self)
        project_exists = os.path.exists(project_path)
        guids = guid_manager()
        
        if fl_object.initialised == False:
                    
            fl_object.vs_framelib_guid = guids.vs("framelib")
            fl_object.vs_framelib_obj_guid = guids.vs("framelib_objects")
            fl_object.vs_max_objects_guid = guids.vs("Max Object Projects")
            fl_object.vs_main_guid = guids.vs_main()
                            
            fl_object.xcode_main_guid = guids.xc("PBXProject", "Project object")
            fl_object.xcode_framelib_guid = guids.xc("PBXNativeTarget", "framelib")
            fl_object.xcode_max_config_guid = guids.xc("PBXFileReference", "Config_FrameLib_Max.xcconfig")
            fl_object.xcode_fileref_lib_guid = guids.xc("PBXFileReference", "libframelib.a")
            fl_object.xcode_fileref_fft_guid = guids.xc("PBXFileReference", "HISSTools_FFT.cpp")
            fl_object.xcode_fileref_ibuffer_guid = guids.xc("PBXFileReference", "ibuffer_access.cpp")
    
            fl_object.xcode_lib_sources_guid = guids.xc_component("PBXNativeTarget", "framelib_objects", "buildPhases", "Sources")
            
            fl_object.initialised = True

        if class_name in fl_object.object_cache:
        
            self.__dict__ = copy.deepcopy(fl_object.object_cache[class_name].__dict__)
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

            self.vs_project_guid = item_regex(project_path, "<ProjectGuid>\{(.*)\}</ProjectGuid>")

            self.xcode_obj_target_guid = guids.xc("PBXNativeTarget", self.max_class_name)

            bounds = section_bounds("PBXTargetDependency") + ["/* Begin PBXTargetDependency", "target = " + self.xcode_obj_target_guid]
            exp = "[\S\s]*\s([^\s]+) /\* PBXTargetDependency \*/ = \{[\S\s]*?\Z"
            self.xcode_obj_package_dep_guid = guids.xc_custom(bounds, exp)
            
            self.xcode_obj_lib_dep_guid = guids.xc_component("PBXNativeTarget", self.max_class_name, "dependencies", "PBXTargetDependency")

            self.xcode_obj_lib_proxy_guid = guids.xc_field("PBXTargetDependency", self.xcode_obj_lib_dep_guid, "targetProxy")
            self.xcode_obj_target_proxy_guid = guids.xc_field("PBXTargetDependency", self.xcode_obj_package_dep_guid, "targetProxy")
            
            self.xcode_obj_sources_guid = guids.xc_component("PBXNativeTarget", self.max_class_name, "buildPhases", "Sources")
            self.xcode_obj_frameworks_guid = guids.xc_component("PBXNativeTarget", self.max_class_name, "buildPhases", "Frameworks")

            class_str = self.max_class_name + ".cpp in Sources"
            object_str = self.object_class + ".cpp in Sources"
            fft_str = "HISSTools_FFT.cpp in Sources"
            ibuffer_str = "ibuffer_access.cpp in Sources"
            lib_str = "libframelib.a in Frameworks"
            
            sources_guid = self.xcode_obj_sources_guid
            frameworks_guid = self.xcode_obj_frameworks_guid
            lib_guid = self.xcode_lib_sources_guid

            self.xcode_obj_file_class_guid = guids.xc_component("PBXSourcesBuildPhase", "Sources", "files", class_str, sources_guid)
            self.xcode_obj_file_object_guid = guids.xc_component("PBXSourcesBuildPhase", "Sources", "files", object_str, sources_guid)
            self.xcode_obj_file_lib_guid = guids.xc_component("PBXFrameworksBuildPhase", "Frameworks", "files", lib_str, frameworks_guid)
            self.xcode_obj_file_object_for_lib_guid = guids.xc_component("PBXSourcesBuildPhase", "Sources", "files", object_str, lib_guid)
            self.xcode_obj_file_fft_guid = guids.xc_component("PBXSourcesBuildPhase", "Sources", "files", fft_str, sources_guid)
            self.xcode_obj_file_ibuffer_guid = guids.xc_component("PBXSourcesBuildPhase", "Sources", "files", ibuffer_str, sources_guid)

            self.xcode_obj_fileref_class_guid = guids.xc("PBXFileReference", self.max_class_name + ".cpp")
            self.xcode_obj_fileref_object_guid = guids.xc("PBXFileReference", self.object_class + ".cpp")
            self.xcode_obj_fileref_header_guid = guids.xc("PBXFileReference", self.object_class + ".h")
            self.xcode_obj_fileref_mxo_guid = guids.xc("PBXFileReference", self.max_class_name + ".mxo")
            
            config_list = "Build configuration list for PBXNativeTarget \"" + self.max_class_name + "\""
            
            self.xcode_obj_config_list_guid = guids.xc("XCConfigurationList", config_list)
            self.xcode_obj_config_dvmt_guid = guids.xc_component("XCConfigurationList", config_list, "buildConfigurations", "Development")
            self.xcode_obj_config_dplt_guid = guids.xc_component("XCConfigurationList", config_list, "buildConfigurations", "Deployment")
            self.xcode_obj_config_test_guid = guids.xc_component("XCConfigurationList", config_list, "buildConfigurations", "Public Testing")
            
        else:
        
            self.vs_project_guid = guids.create_vs()
            
            self.xcode_obj_target_guid = guids.create_xc()
            self.xcode_obj_package_dep_guid = guids.create_xc()
            self.xcode_obj_lib_dep_guid = guids.create_xc()

            self.xcode_obj_lib_proxy_guid = guids.create_xc()
            self.xcode_obj_target_proxy_guid = guids.create_xc()
            
            self.xcode_obj_sources_guid = guids.create_xc()
            self.xcode_obj_frameworks_guid = guids.create_xc()

            self.xcode_obj_file_class_guid = guids.create_xc()
            self.xcode_obj_file_object_guid = guids.create_xc()
            self.xcode_obj_file_lib_guid = guids.create_xc()
            self.xcode_obj_file_object_for_lib_guid = guids.create_xc()
            
            if source_type == "fft":
                self.xcode_obj_file_fft_guid = guids.create_xc()
                self.xcode_obj_file_ibuffer_guid = ""
            elif source_type == "ibuffer":
                self.xcode_obj_file_fft_guid = ""
                self.xcode_obj_file_ibuffer_guid = guids.create_xc()
            else:
                self.xcode_obj_file_fft_guid = ""
                self.xcode_obj_file_ibuffer_guid = ""
                
            self.xcode_obj_fileref_class_guid = guids.create_xc()
            self.xcode_obj_fileref_object_guid = guids.create_xc()
            self.xcode_obj_fileref_header_guid = guids.create_xc()
            self.xcode_obj_fileref_mxo_guid = guids.create_xc()
            
            self.xcode_obj_config_list_guid = guids.create_xc()
            self.xcode_obj_config_dvmt_guid = guids.create_xc()
            self.xcode_obj_config_dplt_guid = guids.create_xc()
            self.xcode_obj_config_test_guid = guids.create_xc()
        
        fl_object.object_cache[class_name] = self
        
        
    @staticmethod
    def create_from_name(class_name: str):
        return fl_object("", class_name, "")

