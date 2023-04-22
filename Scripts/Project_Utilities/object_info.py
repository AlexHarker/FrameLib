
class fl_object:

    vs_fl_guid = ""
    vs_fl_objects_guid = ""
    vs_fl_max_objects_guid = ""
    vs_main_guid = ""
            
    xcode_main_guid = ""
    xcode_framelib_guid = ""
    xcode_max_config_guid = ""
    xcode_fileref_lib_guid = ""
    
    initialised = False
    
    def __init__(self, object_class: str, class_name: str, category: str):
        
        from . file_util import get_vs_guid
        from . file_util import get_xcode_guid
        from . file_util import get_guid_regex
        from . file_util import create_xcode_guid
        from . path_util import fl_paths

        self.object_class = object_class
        self.max_class_name = class_name
        self.pd_class_name = class_name
        self.category = category
        self.guid = ""
        
        if fl_object.initialised == False:
        
            fl_object.vs_fl_guid = get_vs_guid(fl_paths().vs_solution(), "framelib")
            fl_object.vs_fl_objects_guid = get_vs_guid(fl_paths().vs_solution(), "framelib_objects")
            fl_object.vs_fl_max_objects_guid = get_vs_guid(fl_paths().vs_solution(),  "Max Object Projects")
            fl_object.vs_main_guid = get_guid_regex(fl_paths().vs_solution(), "Project\(\"\{(.*)\}\"\) = \"framelib\"")
                
            fl_object.xcode_main_guid = get_xcode_guid(fl_paths().xcode_pbxproj(), "Project object")
            fl_object.xcode_framelib_guid = get_xcode_guid(fl_paths().xcode_pbxproj(), "framelib")
            fl_object.xcode_max_config_guid = get_xcode_guid(fl_paths().xcode_pbxproj(), "Config_FrameLib_Max.xcconfig")
            fl_object.xcode_fileref_lib_guid = get_xcode_guid(fl_paths().xcode_pbxproj(), "libframelib.a")
            
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
        self.xcode_obj_file_lib_guid = create_xcode_guid()
        self.xcode_obj_fileref_mxo_guid = create_xcode_guid()
        self.xcode_obj_file_object_for_lib_guid = create_xcode_guid()
        
        self.xcode_obj_config_list_guid = create_xcode_guid()
        self.xcode_obj_config_dvmt_guid = create_xcode_guid()
        self.xcode_obj_config_dplt_guid = create_xcode_guid()
        self.xcode_obj_config_test_guid = create_xcode_guid()
