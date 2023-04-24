
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
        from . file_util import section_regex
        from . guid_util import get_vs_guid
        from . guid_util import get_xcode_guid
        from . guid_util import get_xcode_component_guid
        from . guid_util import get_xcode_field_guid
        from . guid_util import create_vs_guid
        from . guid_util import create_xcode_guid
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
                
        if fl_object.initialised == False:
                    
            fl_object.vs_framelib_guid = get_vs_guid("framelib")
            fl_object.vs_framelib_obj_guid = get_vs_guid("framelib_objects")
            fl_object.vs_max_objects_guid = get_vs_guid("Max Object Projects")
            fl_object.vs_main_guid = item_regex(fl_paths().vs_solution(), "Project\(\"\{(.*)\}\"\) = \"framelib\"")
                            
            fl_object.xcode_main_guid = get_xcode_guid("PBXProject", "Project object")
            fl_object.xcode_framelib_guid = get_xcode_guid("PBXNativeTarget", "framelib")
            fl_object.xcode_max_config_guid = get_xcode_guid("PBXFileReference", "Config_FrameLib_Max.xcconfig")
            fl_object.xcode_fileref_lib_guid = get_xcode_guid("PBXFileReference", "libframelib.a")
            fl_object.xcode_fileref_fft_guid = get_xcode_guid("PBXFileReference", "HISSTools_FFT.cpp")
            fl_object.xcode_fileref_ibuffer_guid = get_xcode_guid("PBXFileReference", "ibuffer_access.cpp")
    
            fl_object.xcode_lib_sources_guid = get_xcode_component_guid("PBXNativeTarget", "framelib_objects", "buildPhases", "Sources")
            
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

            self.guid = item_regex(project_path, "<ProjectGuid>\{(.*)\}</ProjectGuid>")

            self.xcode_obj_target_guid = get_xcode_guid("PBXNativeTarget", self.max_class_name)

            bounds = section_bounds("PBXTargetDependency") + ["/* Begin PBXTargetDependency", "target = " + self.xcode_obj_target_guid]
            exp = "[\S\s]*\s([^\s]+) /\* PBXTargetDependency \*/ = \{[\S\s]*?\Z"
            self.xcode_obj_package_dep_guid = section_regex(fl_paths().xcode_pbxproj(), bounds, exp)
            
            self.xcode_obj_lib_dep_guid = get_xcode_component_guid("PBXNativeTarget", self.max_class_name, "dependencies", "PBXTargetDependency")

            self.xcode_obj_lib_proxy_guid = get_xcode_field_guid("PBXTargetDependency", self.xcode_obj_lib_dep_guid, "targetProxy")
            self.xcode_obj_target_proxy_guid = get_xcode_field_guid("PBXTargetDependency", self.xcode_obj_package_dep_guid, "targetProxy")
            
            self.xcode_obj_sources_guid = get_xcode_component_guid("PBXNativeTarget", self.max_class_name, "buildPhases", "Sources")
            self.xcode_obj_frameworks_guid = get_xcode_component_guid("PBXNativeTarget", self.max_class_name, "buildPhases", "Frameworks")

            class_str = self.max_class_name + ".cpp in Sources"
            object_str = self.object_class + ".cpp in Sources"
            fft_str = "HISSTools_FFT.cpp in Sources"
            ibuffer_str = "ibuffer_access.cpp in Sources"
            lib_str = "libframelib.a in Frameworks"
            
            sources_guid = self.xcode_obj_sources_guid
            frameworks_guid = self.xcode_obj_frameworks_guid
            lib_guid = self.xcode_lib_sources_guid

            self.xcode_obj_file_class_guid = get_xcode_component_guid("PBXSourcesBuildPhase", "Sources", "files", class_str, sources_guid)
            self.xcode_obj_file_object_guid = get_xcode_component_guid("PBXSourcesBuildPhase", "Sources", "files", object_str, sources_guid)
            self.xcode_obj_file_lib_guid = get_xcode_component_guid("PBXFrameworksBuildPhase", "Frameworks", "files", lib_str, frameworks_guid)
            self.xcode_obj_file_object_for_lib_guid = get_xcode_component_guid("PBXSourcesBuildPhase", "Sources", "files", object_str, lib_guid)
            self.xcode_obj_file_fft_guid = get_xcode_component_guid("PBXSourcesBuildPhase", "Sources", "files", fft_str, sources_guid)
            self.xcode_obj_file_ibuffer_guid = get_xcode_component_guid("PBXSourcesBuildPhase", "Sources", "files", ibuffer_str, sources_guid)

            self.xcode_obj_fileref_class_guid = get_xcode_guid("PBXFileReference", self.max_class_name + ".cpp")
            self.xcode_obj_fileref_object_guid = get_xcode_guid("PBXFileReference", self.object_class + ".cpp")
            self.xcode_obj_fileref_header_guid = get_xcode_guid("PBXFileReference", self.object_class + ".h")
            self.xcode_obj_fileref_mxo_guid = get_xcode_guid("PBXFileReference", self.max_class_name + ".mxo")
            
            config_list = "Build configuration list for PBXNativeTarget \"" + self.max_class_name + "\""
            
            self.xcode_obj_config_list_guid = get_xcode_guid("XCConfigurationList", config_list)
            self.xcode_obj_config_dvmt_guid = get_xcode_component_guid("XCConfigurationList", config_list, "buildConfigurations", "Development")
            self.xcode_obj_config_dplt_guid = get_xcode_component_guid("XCConfigurationList", config_list, "buildConfigurations", "Deployment")
            self.xcode_obj_config_test_guid = get_xcode_component_guid("XCConfigurationList", config_list, "buildConfigurations", "Public Testing")
            
        else:
        
            self.guid = create_vs_guid()
            
            self.xcode_obj_target_guid = create_xcode_guid()
            self.xcode_obj_package_dep_guid = create_xcode_guid()
            self.xcode_obj_lib_dep_guid = create_xcode_guid()

            self.xcode_obj_lib_proxy_guid = create_xcode_guid()
            self.xcode_obj_target_proxy_guid = create_xcode_guid()
            
            self.xcode_obj_sources_guid = create_xcode_guid()
            self.xcode_obj_frameworks_guid = create_xcode_guid()

            self.xcode_obj_file_class_guid = create_xcode_guid()
            self.xcode_obj_file_object_guid = create_xcode_guid()
            self.xcode_obj_file_lib_guid = create_xcode_guid()
            self.xcode_obj_file_object_for_lib_guid = create_xcode_guid()
            
            if source_type == "fft":
                self.xcode_obj_file_fft_guid = create_xcode_guid()
                self.xcode_obj_file_ibuffer_guid = ""
            elif source_type == "ibuffer":
                self.xcode_obj_file_fft_guid = ""
                self.xcode_obj_file_ibuffer_guid = create_xcode_guid()
            else:
                self.xcode_obj_file_fft_guid = ""
                self.xcode_obj_file_ibuffer_guid = ""
                
            self.xcode_obj_fileref_class_guid = create_xcode_guid()
            self.xcode_obj_fileref_object_guid = create_xcode_guid()
            self.xcode_obj_fileref_header_guid = create_xcode_guid()
            self.xcode_obj_fileref_mxo_guid = create_xcode_guid()
            
            self.xcode_obj_config_list_guid = create_xcode_guid()
            self.xcode_obj_config_dvmt_guid = create_xcode_guid()
            self.xcode_obj_config_dplt_guid = create_xcode_guid()
            self.xcode_obj_config_test_guid = create_xcode_guid()
        
        fl_object.object_cache[class_name] = self
        
        
    @staticmethod
    def create_from_name(class_name: str):
        return fl_object("", class_name, "")

