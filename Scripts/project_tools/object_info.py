
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
        
        gm = guid_manager()
        
        # Deal with static guids
        
        if fl_object.initialised == False:
                    
            static_guids = fl_object.static_guids
            
            # Static VS guids
            
            static_guids["vs_framelib_guid"] = gm.vs("framelib")
            static_guids["vs_framelib_obj_guid"] = gm.vs("framelib_objects")
            static_guids["vs_max_objects_guid"] = gm.vs("Max Object Projects")
            static_guids["vs_main_guid"] = gm.vs_main()
                            
            # Static Xcode guids

            static_guids["xc_main_guid"] = gm.xc("PBXProject", "Project object")
            static_guids["xc_framelib_guid"] = gm.xc("PBXNativeTarget", "framelib")
            static_guids["xc_max_config_guid"] = gm.xc("PBXFileReference", "Config_FrameLib_Max.xcconfig")
            static_guids["xc_fileref_lib_guid"] = gm.xc("PBXFileReference", "libframelib.a")
            static_guids["xc_fileref_fft_guid"] = gm.xc("PBXFileReference", "HISSTools_FFT.cpp")
            static_guids["xc_fileref_ibuffer_guid"] = gm.xc("PBXFileReference", "ibuffer_access.cpp")
            
            sources_guid = gm.xc_component("PBXNativeTarget", "framelib_objects", "buildPhases", "Sources")
            static_guids["xc_lib_sources_guid"] = sources_guid
            
            fl_object.initialised = True

        if class_name in fl_object.object_cache:
        
            # Retrieve info from the cache if it exists
            
            self.info = fl_object.object_cache[class_name]
            return
            
        elif project_exists:
        
            project = rw_file(project_path)

            # Find category
            
            info["category"] = category = regex_search(project.data, "FrameLib_Max_Objects\\\\(.*)\\\\fl")

            max_object_path = fl_paths().max_source(self)
            max_object = rw_file(max_object_path)
            
            # Determine the object class
            
            info["object_class"] = regex_search(max_object.data, "FrameLib_MaxClass_Expand<(.*)>").split(",")[0]

            if info["object_class"] == "":
                info["object_class"] = regex_search(max_object.data, "FrameLib_MaxClass<(.*)>")

            if info["object_class"] == "":
                info["object_class"] = regex_search(max_object.data, "FrameLib_MaxClass_ExprParsed<(.*)>")

            # Rework basic info
            
            info["object_class_name_upper"] = info["object_class"].upper()
            info["object_class_file"] = regex_search(info["object_class"], "([^:]*)")
            
            # VS project guid

            info["vs_project_guid"] = regex_search(project.data, "<ProjectGuid>\{(.*)\}</ProjectGuid>")

            # Xcode misc guids

            tgt_tag = "PBXNativeTarget"
            dep_tag = "PBXTargetDependency"
            
            info["xc_obj_target_guid"] = gm.xc(tgt_tag, class_name)
            info["xc_obj_package_dep_guid"] = gm.xc_target_dependency(info["xc_obj_target_guid"])
            info["xc_obj_lib_dep_guid"] = gm.xc_component(tgt_tag, class_name, "dependencies", dep_tag)

            # Xcode proxy guids
            
            dep_tag = "PBXTargetDependency"
            prx_tag = "targetProxy"
            
            info["xc_obj_lib_proxy_guid"] = gm.xc_field(dep_tag, info["xc_obj_lib_dep_guid"], prx_tag)
            info["xc_obj_target_proxy_guid"] = gm.xc_field(dep_tag, info["xc_obj_package_dep_guid"], prx_tag)
            
            # Xcode build phases guids
                        
            tgt_tag = "PBXNativeTarget"
            phs_tag = "buildPhases"
            
            info["xc_obj_sources_guid"] = gm.xc_component(tgt_tag, class_name, phs_tag, "Sources")
            info["xc_obj_frameworks_guid"] = gm.xc_component(tgt_tag, class_name, phs_tag, "Frameworks")

            # Xcode file guids

            src_tag = "PBXSourcesBuildPhase"
            fwk_tag = "PBXFrameworksBuildPhase"
            scn_tag = "Sources"
            fkn_tag = "Frameworks"
            
            class_str = class_name + ".cpp in Sources"
            object_str = info["object_class_file"] + ".cpp in Sources"
            fft_str = "HISSTools_FFT.cpp in Sources"
            ibuffer_str = "ibuffer_access.cpp in Sources"
            lib_str = "libframelib.a in Frameworks"
            
            src_guid = info["xc_obj_sources_guid"]
            fwk_guid = info["xc_obj_frameworks_guid"]
            lib_guid = fl_object.static_guids["xc_lib_sources_guid"]

            info["xc_obj_file_class_guid"] = gm.xc_component(src_tag, scn_tag, "files", class_str, src_guid)
            info["xc_obj_file_object_guid"] = gm.xc_component(src_tag, scn_tag, "files", object_str, src_guid)
            info["xc_obj_file_lib_guid"] = gm.xc_component(fwk_tag, fkn_tag, "files", lib_str, fwk_guid)
            info["xc_obj_file_object_lib_guid"] = gm.xc_component(src_tag, scn_tag, "files", object_str, lib_guid)
            info["xc_obj_file_fft_guid"] = gm.xc_component(src_tag, scn_tag, "files", fft_str, src_guid)
            info["xc_obj_file_ibuffer_guid"] = gm.xc_component(src_tag, scn_tag, "files", ibuffer_str, src_guid)

            # Xcode file reference guids
    
            fref_tag = "PBXFileReference"
            
            info["xc_obj_fileref_class_guid"] = gm.xc(fref_tag, class_name + ".cpp")
            info["xc_obj_fileref_object_guid"] = gm.xc(fref_tag, info["object_class_file"] + ".cpp")
            info["xc_obj_fileref_header_guid"] = gm.xc(fref_tag, info["object_class_file"] + ".h")
            info["xc_obj_fileref_mxo_guid"] = gm.xc(fref_tag, class_name + ".mxo")
            
            # Xcode config guids

            cnf_tag = "XCConfigurationList"
            cnf_list = "Build configuration list for PBXNativeTarget \"" + class_name + "\""
            cnf_build = "buildConfigurations"
            
            info["xc_obj_config_list_guid"] = gm.xc(cnf_tag, cnf_list)
            info["xc_obj_config_dvmt_guid"] = gm.xc_component(cnf_tag, cnf_list, cnf_build, "Development")
            info["xc_obj_config_dplt_guid"] = gm.xc_component(cnf_tag, cnf_list, cnf_build, "Deployment")
            info["xc_obj_config_test_guid"] = gm.xc_component(cnf_tag, cnf_list, cnf_build, "Public Testing")
            
        else:
        
            # VS project guid
                    
            info["vs_project_guid"] = gm.create_vs()
            
            # Xcode misc guids

            info["xc_obj_target_guid"] = gm.create_xc()
            info["xc_obj_package_dep_guid"] = gm.create_xc()
            info["xc_obj_lib_dep_guid"] = gm.create_xc()

            # Xcode proxy guids

            info["xc_obj_lib_proxy_guid"] = gm.create_xc()
            info["xc_obj_target_proxy_guid"] = gm.create_xc()
            
            # Xcode build phases guids

            info["xc_obj_sources_guid"] = gm.create_xc()
            info["xc_obj_frameworks_guid"] = gm.create_xc()

            # Xcode file guids
            
            info["xc_obj_file_class_guid"] = gm.create_xc()
            info["xc_obj_file_object_guid"] = gm.create_xc()
            info["xc_obj_file_lib_guid"] = gm.create_xc()
            info["xc_obj_file_object_lib_guid"] = gm.create_xc()
            
            if source_type == "fft":
                info["xc_obj_file_fft_guid"] = gm.create_xc()
                info["xc_obj_file_ibuffer_guid"] = ""
            elif source_type == "ibuffer":
                info["xc_obj_file_fft_guid"] = ""
                info["xc_obj_file_ibuffer_guid"] = gm.create_xc()
            else:
                info["xc_obj_file_fft_guid"] = ""
                info["xc_obj_file_ibuffer_guid"] = ""
                
            # Xcode file reference guids

            info["xc_obj_fileref_class_guid"] = gm.create_xc()
            info["xc_obj_fileref_object_guid"] = gm.create_xc()
            info["xc_obj_fileref_header_guid"] = gm.create_xc()
            info["xc_obj_fileref_mxo_guid"] = gm.create_xc()
            
            # Xcode config guids
                        
            info["xc_obj_config_list_guid"] = gm.create_xc()
            info["xc_obj_config_dvmt_guid"] = gm.create_xc()
            info["xc_obj_config_dplt_guid"] = gm.create_xc()
            info["xc_obj_config_test_guid"] = gm.create_xc()
        
        # Find the category groups
                
        info["xc_group_object_guid"] = gm.xc_component("PBXGroup", "Objects FrameLib", "children", category)
        info["xc_group_max_guid"] = gm.xc_component("PBXGroup", "Objects Max", "children", category)
      
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

