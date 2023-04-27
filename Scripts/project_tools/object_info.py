
import json
from pathlib import Path

class fl_object:
    
    object_cache = {}
    static_guids = {}
    
    initialised = False
    
    def __getitem__(self, key):
        if key.lower() in self.info:
            return self.info[key]
        else:
            return fl_object.static_guids[key]
        
    
    def __init__(self, object_class: str, class_name: str, category: str, source_type: str = ""):
        
        from . path_util import fl_paths
        from . file_util import regex_search
        from . file_util import rw_file
        from . guid_util import guid_manager
        
        self.info = {}
        info = self.info
        
        # Basic object info
        
        info["object_class"] = object_class
        info["object_class_name_upper"] = object_class.upper()
        info["object_class_file"] = regex_search(object_class, "([^:]*)")
        info["max_class_name"] = class_name
        info["pd_class_name"] = class_name
        info["category"] = category
        
        # Store the project path and see if it already exists
        
        project_path = fl_paths().vs_max_project(self)
        project_exists = Path(project_path).exists()
        
        # The guid manager
        
        guids = guid_manager()
        
        # Deal with static guids
        
        if fl_object.initialised == False:
                    
            static_guids = fl_object.static_guids
            
            # Static VS guids
            
            static_guids["vs_framelib_guid"] = guids.vs("framelib")
            static_guids["vs_framelib_obj_guid"] = guids.vs("framelib_objects")
            static_guids["vs_max_objects_guid"] = guids.vs("Max Object Projects")
            static_guids["vs_main_guid"] = guids.vs_main()
                            
            # Static Xcode guids

            static_guids["xc_main_guid"] = guids.xc("PBXProject", "Project object")
            static_guids["xc_framelib_guid"] = guids.xc("PBXNativeTarget", "framelib")
            static_guids["xc_max_config_guid"] = guids.xc("PBXFileReference", "Config_FrameLib_Max.xcconfig")
            static_guids["xc_fileref_lib_guid"] = guids.xc("PBXFileReference", "libframelib.a")
            static_guids["xc_fileref_fft_guid"] = guids.xc("PBXFileReference", "HISSTools_FFT.cpp")
            static_guids["xc_fileref_ibuffer_guid"] = guids.xc("PBXFileReference", "ibuffer_access.cpp")
            static_guids["xc_lib_sources_guid"] = guids.xc_component("PBXNativeTarget", "framelib_objects", "buildPhases", "Sources")
            
            fl_object.initialised = True

        if class_name in fl_object.object_cache:
        
            # Retrieve info from the cache if it exists
            
            self.info = fl_object.object_cache[class_name]
            return
            
        elif project_exists:
        
            project = rw_file(project_path)

            info["category"] = regex_search(project.data, "FrameLib_Max_Objects\\\\(.*)\\\\fl")

            max_object_path = fl_paths().max_source(self)
            max_object = rw_file(max_object_path)
            
            info["object_class"] = regex_search(max_object.data, "FrameLib_MaxClass_Expand<(.*)>").split(",")[0]

            if info["object_class"] == "":
                info["object_class"] = regex_search(max_object.data, "FrameLib_MaxClass<(.*)>")

            if info["object_class"] == "":
                info["object_class"] = regex_search(max_object.data, "FrameLib_MaxClass_ExprParsed<(.*)>")

            info["object_class_name_upper"] = info["object_class"].upper()
            info["object_class_file"] = regex_search(info["object_class"], "([^:]*)")
            
            info["vs_project_guid"] = regex_search(project.data, "<ProjectGuid>\{(.*)\}</ProjectGuid>")

            info["xc_obj_target_guid"] = guids.xc("PBXNativeTarget", info["max_class_name"])

            info["xc_obj_package_dep_guid"] = guids.xc_target_dependency(info["xc_obj_target_guid"])
            info["xc_obj_lib_dep_guid"] = guids.xc_component("PBXNativeTarget", info["max_class_name"], "dependencies", "PBXTargetDependency")

            info["xc_obj_lib_proxy_guid"] = guids.xc_field("PBXTargetDependency", info["xc_obj_lib_dep_guid"], "targetProxy")
            info["xc_obj_target_proxy_guid"] = guids.xc_field("PBXTargetDependency", info["xc_obj_package_dep_guid"], "targetProxy")
            
            info["xc_obj_sources_guid"] = guids.xc_component("PBXNativeTarget", info["max_class_name"], "buildPhases", "Sources")
            info["xc_obj_frameworks_guid"] = guids.xc_component("PBXNativeTarget", info["max_class_name"], "buildPhases", "Frameworks")

            class_str = info["max_class_name"] + ".cpp in Sources"
            object_str = info["object_class_file"] + ".cpp in Sources"
            fft_str = "HISSTools_FFT.cpp in Sources"
            ibuffer_str = "ibuffer_access.cpp in Sources"
            lib_str = "libframelib.a in Frameworks"
            
            sources_guid = info["xc_obj_sources_guid"]
            frameworks_guid = info["xc_obj_frameworks_guid"]
            lib_guid = fl_object.static_guids["xc_lib_sources_guid"]

            info["xc_obj_file_class_guid"] = guids.xc_component("PBXSourcesBuildPhase", "Sources", "files", class_str, sources_guid)
            info["xc_obj_file_object_guid"] = guids.xc_component("PBXSourcesBuildPhase", "Sources", "files", object_str, sources_guid)
            info["xc_obj_file_lib_guid"] = guids.xc_component("PBXFrameworksBuildPhase", "Frameworks", "files", lib_str, frameworks_guid)
            info["xc_obj_file_object_for_lib_guid"] = guids.xc_component("PBXSourcesBuildPhase", "Sources", "files", object_str, lib_guid)
            info["xc_obj_file_fft_guid"] = guids.xc_component("PBXSourcesBuildPhase", "Sources", "files", fft_str, sources_guid)
            info["xc_obj_file_ibuffer_guid"] = guids.xc_component("PBXSourcesBuildPhase", "Sources", "files", ibuffer_str, sources_guid)

            info["xc_obj_fileref_class_guid"] = guids.xc("PBXFileReference", info["max_class_name"] + ".cpp")
            info["xc_obj_fileref_object_guid"] = guids.xc("PBXFileReference", info["object_class_file"] + ".cpp")
            info["xc_obj_fileref_header_guid"] = guids.xc("PBXFileReference", info["object_class_file"] + ".h")
            info["xc_obj_fileref_mxo_guid"] = guids.xc("PBXFileReference", info["max_class_name"] + ".mxo")
            
            config_list = "Build configuration list for PBXNativeTarget \"" + info["max_class_name"] + "\""
            
            info["xc_obj_config_list_guid"] = guids.xc("XCConfigurationList", config_list)
            info["xc_obj_config_dvmt_guid"] = guids.xc_component("XCConfigurationList", config_list, "buildConfigurations", "Development")
            info["xc_obj_config_dplt_guid"] = guids.xc_component("XCConfigurationList", config_list, "buildConfigurations", "Deployment")
            info["xc_obj_config_test_guid"] = guids.xc_component("XCConfigurationList", config_list, "buildConfigurations", "Public Testing")
            
        else:
        
            # VS project guid
                    
            info["vs_project_guid"] = guids.create_vs()
            
            # Xcode misc guids

            info["xc_obj_target_guid"] = guids.create_xc()
            info["xc_obj_package_dep_guid"] = guids.create_xc()
            info["xc_obj_lib_dep_guid"] = guids.create_xc()

            # Xcode proxy guids

            info["xc_obj_lib_proxy_guid"] = guids.create_xc()
            info["xc_obj_target_proxy_guid"] = guids.create_xc()
            
            # Xcode build phases guids

            info["xc_obj_sources_guid"] = guids.create_xc()
            info["xc_obj_frameworks_guid"] = guids.create_xc()

            # Xcode file guids
            
            info["xc_obj_file_class_guid"] = guids.create_xc()
            info["xc_obj_file_object_guid"] = guids.create_xc()
            info["xc_obj_file_lib_guid"] = guids.create_xc()
            info["xc_obj_file_object_for_lib_guid"] = guids.create_xc()
            
            if source_type == "fft":
                info["xc_obj_file_fft_guid"] = guids.create_xc()
                info["xc_obj_file_ibuffer_guid"] = ""
            elif source_type == "ibuffer":
                info["xc_obj_file_fft_guid"] = ""
                info["xc_obj_file_ibuffer_guid"] = guids.create_xc()
            else:
                info["xc_obj_file_fft_guid"] = ""
                info["xc_obj_file_ibuffer_guid"] = ""
                
            # Xcode file reference guids

            info["xc_obj_fileref_class_guid"] = guids.create_xc()
            info["xc_obj_fileref_object_guid"] = guids.create_xc()
            info["xc_obj_fileref_header_guid"] = guids.create_xc()
            info["xc_obj_fileref_mxo_guid"] = guids.create_xc()
            
            # Xcode config guids
                        
            info["xc_obj_config_list_guid"] = guids.create_xc()
            info["xc_obj_config_dvmt_guid"] = guids.create_xc()
            info["xc_obj_config_dplt_guid"] = guids.create_xc()
            info["xc_obj_config_test_guid"] = guids.create_xc()
        
        # Find the category groups
                
        info["xc_group_object_guid"] = guids.xc_component("PBXGroup", "Objects FrameLib", "children", info["category"])
        info["xc_group_max_guid"] = guids.xc_component("PBXGroup", "Objects Max", "children", info["category"])
      
        fl_object.object_cache[class_name] = info
        
        
    # Helper to add XC guids to a key
    
    def __set_xc(self, key: str):

        from . guid_util import guid_manager
        
        guid = guid_manager().create_xc()
        self.info[key] = guid
        
        return guid
        
        
    # Methods to add missing categories
    
    def add_xc_object_group(self):
        return self.__set_xc("xc_group_object_guid")
        
        
    def add_xc_max_group(self):
        return self.__set_xc("xc_group_max_guid")

    
    # Method to determine if this max class owns the file class file or not
    
    def object_file_external(self):
        return self["object_class"] != self["object_class_file"]
    
    
    @staticmethod
    def create_from_name(class_name: str):
        return fl_object("", class_name, "")
        
        
    @staticmethod
    def save_cache():

        from . path_util import fl_paths
        
        path = Path(fl_paths().cache_path())
        path.parent.mkdir(parents = True, exist_ok = True)

        with open(path.as_posix(), "w+") as f:
            json.dump(fl_object.object_cache, f, indent=4)
        
        
    @staticmethod
    def load_cache():
    
        from . path_util import fl_paths

        if Path(fl_paths().cache_path()).exists():
            with open(fl_paths().cache_path(), "r") as f:
                fl_object.object_cache = json.load(f)

