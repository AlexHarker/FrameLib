
class fl_object:

    vs_fl_guid = ""
    vs_fl_objects_guid = ""
    vs_fl_max_objects_guid = ""
    vs_main_guid = ""
            
    xcode_main_guid = ""
    xcode_framelib_guid = ""
    xcode_max_config_guid = ""
    xcode_fileref_lib_guid = ""
    xcode_lib_sources_guid = ""
    
    initialised = False
    
    def __init__(self, object_class: str, class_name: str, category: str):
        
        from . path_util import fl_paths
        from . file_util import section_regex
        from . xcode_util import item_bounds
        from . guid_util import get_guid_regex
        from . guid_util import get_vs_guid
        from . guid_util import get_xcode_guid
        from . guid_util import create_vs_guid
        from . guid_util import create_xcode_guid

        self.object_class = object_class
        self.max_class_name = class_name
        self.pd_class_name = class_name
        self.category = category
        self.guid = create_vs_guid()
        
        if fl_object.initialised == False:
        
            vs_solution_path = fl_paths().vs_solution()
            
            fl_object.vs_fl_guid = get_vs_guid(vs_solution_path, "framelib")
            fl_object.vs_fl_objects_guid = get_vs_guid(vs_solution_path, "framelib_objects")
            fl_object.vs_fl_max_objects_guid = get_vs_guid(vs_solution_path,  "Max Object Projects")
            fl_object.vs_main_guid = get_guid_regex(vs_solution_path, "Project\(\"\{(.*)\}\"\) = \"framelib\"")
                
            xcode_bpxproj_path = fl_paths().xcode_pbxproj()
            
            fl_object.xcode_main_guid = get_xcode_guid(xcode_bpxproj_path, "PBXProject", "Project object")
            fl_object.xcode_framelib_guid = get_xcode_guid(xcode_bpxproj_path, "PBXNativeTarget", "framelib")
            fl_object.xcode_max_config_guid = get_xcode_guid(xcode_bpxproj_path, "PBXFileReference", "Config_FrameLib_Max.xcconfig")
            fl_object.xcode_fileref_lib_guid = get_xcode_guid(xcode_bpxproj_path, "PBXFileReference", "libframelib.a")
            
            fl_object.xcode_lib_sources_guid = section_regex(xcode_bpxproj_path, item_bounds("framelib_objects"), "([^\s].*) /\* Sources \*/")
            
            fl_object.initialised = True

        self.xcode_obj_target_guid = create_xcode_guid()
        self.xcode_obj_package_dep_guid = create_xcode_guid()
        self.xcode_obj_lib_dep_guid = create_xcode_guid()

        self.xcode_obj_lib_proxy_guid = create_xcode_guid()
        self.xcode_obj_target_proxy_guid = create_xcode_guid()
        
        self.xcode_obj_sources_guid = create_xcode_guid()
        self.xcode_obj_frameworks_guid = create_xcode_guid()

        self.xcode_obj_file_class_guid = create_xcode_guid()
        self.xcode_obj_fileref_class_guid = create_xcode_guid()
        self.xcode_obj_file_object_guid = create_xcode_guid()
        self.xcode_obj_fileref_object_guid = create_xcode_guid()
        self.xcode_obj_fileref_header_guid = create_xcode_guid()
        self.xcode_obj_file_lib_guid = create_xcode_guid()
        self.xcode_obj_fileref_mxo_guid = create_xcode_guid()
        self.xcode_obj_file_object_for_lib_guid = create_xcode_guid()
        
        self.xcode_obj_config_list_guid = create_xcode_guid()
        self.xcode_obj_config_dvmt_guid = create_xcode_guid()
        self.xcode_obj_config_dplt_guid = create_xcode_guid()
        self.xcode_obj_config_test_guid = create_xcode_guid()
