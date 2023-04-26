
from . import file_util
from . path_util import fl_paths
from . object_info import fl_object

import copy


def section_bounds(section: str):
    return ["/* Begin " + section + " section */", "/* End " + section + " section */"]


def item_bounds(name: str, guid: str = None):
    if guid is None:
        return ["/* " + name + " */ = {", "};"]
    else:
        return [guid + " /* " + name + " */ = {", "};"]
        

def list_bounds(name: str):
    return [name + " = (", "\t\t\t);"]


def scheme_bounds():
    return ["      <BuildActionEntries>", "      </BuildActionEntries>"]
    
    
class fl_pbxproj:

    def __init__(self):
    
        self.pbxproj = file_util.rw_file(fl_paths().xc_pbxproj())
        self.scheme = file_util.rw_file(fl_paths().xc_scheme())
       
       
    def project_modify(self, object_info: fl_object, template: str, bounds: list, add: bool):
    
        contents = file_util.templated_string(fl_paths().xc_template(template), object_info)
        self.pbxproj.data = file_util.modify(self.pbxproj.data, contents, bounds, add)
    
    
    def scheme_modify(self, object_info: fl_object, add: bool):
    
        contents = file_util.templated_string(fl_paths().xc_template("xcscheme"), object_info)
        self.scheme.data = file_util.modify(self.scheme.data, contents, scheme_bounds(), add)

      
    def project_modify_section(self, object_info: fl_object, template: str, section: str, add: bool):
        self.project_modify(object_info, template, section_bounds(section), add)
   
   
    def update(self, object_info: fl_object, add: bool):

        source_exists = fl_paths().object_source_exists(object_info)
        header_exists = fl_paths().object_header_exists(object_info)
        external_object = object_info.object_file_external()
        category = object_info["category"]
        
        self.project_modify_section(object_info, "file_class", "PBXBuildFile", add)
        
        if source_exists:
            self.project_modify_section(object_info, "file_object", "PBXBuildFile", add)
            
        if source_exists and not external_object:
            self.project_modify_section(object_info, "file_object_for_lib", "PBXBuildFile", add)
        
        self.project_modify_section(object_info, "file_lib", "PBXBuildFile", add)
        
        self.project_modify_section(object_info, "fileref_product", "PBXFileReference", add)
        self.project_modify_section(object_info, "fileref_class", "PBXFileReference", add)
        
        if source_exists and not external_object:
            self.project_modify_section(object_info, "fileref_object", "PBXFileReference", add)
    
        if header_exists and not external_object:
            self.project_modify_section(object_info, "fileref_header", "PBXFileReference", add)
        
        self.project_modify_section(object_info, "proxy_lib", "PBXContainerItemProxy", add)
        self.project_modify_section(object_info, "proxy_target", "PBXContainerItemProxy", add)

        if source_exists:
            sources_phase = "phase_sources"
        else:
            sources_phase = "phase_sources_single"
            
        if object_info["xc_obj_file_fft_guid"] != "":
            self.project_modify_section(object_info, "file_fft", "PBXBuildFile", add)
            sources_phase = "phase_sources_fft"
        elif object_info["xc_obj_file_ibuffer_guid"] != "":
            self.project_modify_section(object_info, "file_ibuffer", "PBXBuildFile", add)
            sources_phase = "phase_sources_ibuffer"

        self.project_modify_section(object_info, sources_phase, "PBXSourcesBuildPhase", add)
        self.project_modify_section(object_info, "phase_frameworks", "PBXFrameworksBuildPhase", add)

        self.project_modify_section(object_info, "target", "PBXNativeTarget", add)

        self.project_modify_section(object_info, "dependency_target", "PBXTargetDependency", add)
        self.project_modify_section(object_info, "dependency_package", "PBXTargetDependency", add)

        self.project_modify_section(object_info, "build_config_develop", "XCBuildConfiguration", add)
        self.project_modify_section(object_info, "build_config_deploy", "XCBuildConfiguration", add)
        self.project_modify_section(object_info, "build_config_test", "XCBuildConfiguration", add)

        self.project_modify_section(object_info, "config_list", "XCConfigurationList", add)
        
        bounds = section_bounds("PBXAggregateTarget") + item_bounds("framelib_max_package") + list_bounds("dependencies")
        self.project_modify(object_info, "ref_dep_package", bounds, add)
        
        bounds = section_bounds("PBXProject") + list_bounds("targets")
        self.project_modify(object_info, "ref_target", bounds, add)
        
        if source_exists and not external_object:
            lib_sources_guid = object_info["xc_lib_sources_guid"]
            bounds = section_bounds("PBXSourcesBuildPhase") + item_bounds("Sources", lib_sources_guid) + list_bounds("files")
            self.project_modify(object_info, "ref_object_for_lib", bounds, add)
        
        bounds = section_bounds("PBXGroup") + item_bounds("Products") + list_bounds("children")

        self.project_modify(object_info, "group_item_product", bounds, add)
              
        # Find the object category group
        
        exp = "([^\s].*) /\* " + category + " \*/"
        
        bounds = section_bounds("PBXGroup") + item_bounds("Objects FrameLib") + list_bounds("children")
        object_guid = file_util.regex_search_section(self.pbxproj.data, bounds, exp)
        temp_info = copy.deepcopy(object_info)
        
        # Make the object category group if it doesn't exist

        if add and object_guid == "" and (header_exists or source_exists):
        
            object_guid = temp_info.add_xc_new_group()
            self.project_modify(temp_info, "group_group", section_bounds("PBXGroup"), add)
            self.project_modify(temp_info, "ref_group", bounds, add)
            
        # Find the max category group

        bounds = section_bounds("PBXGroup") + item_bounds("Objects Max") + list_bounds("children")
        max_guid = file_util.regex_search_section(self.pbxproj.data, bounds, exp)
        
        # Make the max category groups if it doesn't exist

        if add and max_guid == "":

            max_guid = temp_info.add_xc_new_group()
            self.project_modify(temp_info, "group_group", section_bounds("PBXGroup"), add)
            self.project_modify(temp_info, "ref_group", bounds, add)

        # Add files to groups
        
        if header_exists and not external_object:
            bounds = section_bounds("PBXGroup") + item_bounds(category, object_guid) + list_bounds("children")
            self.project_modify(object_info, "group_item_header", bounds, add)
       
        if source_exists and not external_object:
            bounds = section_bounds("PBXGroup") + item_bounds(category, object_guid) + list_bounds("children")
            self.project_modify(object_info, "group_item_object", bounds, add)
        
        bounds = section_bounds("PBXGroup") + item_bounds(category, max_guid) + list_bounds("children")
        self.project_modify(object_info, "group_item_class", bounds, add)
        
        self.scheme_modify(object_info, add)
        
        # Flush
        
        self.pbxproj.flush()
        self.scheme.flush()
