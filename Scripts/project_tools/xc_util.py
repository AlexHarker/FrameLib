
from . path_util import fl_paths
from . object_info import fl_object

from . import file_util


# Helpers to generate bounds for sections / items / lists in the Xcode project

def section_bounds(section: str):
    return ["/* Begin " + section + " section */", "/* End " + section + " section */"]


def item_bounds(name: str, guid: str = None):
    if guid is None:
        return ["/* " + name + " */ = {", "};"]
    else:
        return [guid + " /* " + name + " */ = {", "};"]
        

def list_bounds(name: str):
    return [name + " = (", "\t\t\t);"]

    
# Helper for bounds relating to the Xcode scheme

def scheme_bounds():
    return ["      <BuildActionEntries>", "      </BuildActionEntries>"]
    
    
class fl_pbxproj:

    def __init__(self):
    
        self.pbxproj = file_util.rw_file(fl_paths().xc_pbxproj())
        self.scheme = file_util.rw_file(fl_paths().xc_scheme())
       
       
    def project_modify(self, object_info: fl_object, template: str, bounds: list, add: bool):
    
        # Add or remove templated contents from the Xcode project
        
        contents = file_util.templated_string(fl_paths().xc_template(template), object_info)
        self.pbxproj.data = file_util.modify(self.pbxproj.data, contents, bounds, add)
    
    
    def project_category_sort(self, section: str, item: str, list_name: str):
    
        # Sort an Xcode project category folder

        bounds = section_bounds(section) + item_bounds(item) + list_bounds(list_name) + ["Complex_Binary"]
        self.pbxproj.data = file_util.sort_section(self.pbxproj.data, bounds, "/\*(.*)\*/")
        
      
    def project_modify_section(self, object_info: fl_object, template: str, section: str, add: bool):
    
        # Modify a named section of the Xcode project
            
        self.project_modify(object_info, template, section_bounds(section), add)
        
    
    def project_modify_nested(self, object_info: fl_object, template: str, section: str, item: str, list_name: str, add: bool, item_guid: str = None):
    
        # Modify a nested list in the Xcode project
            
        bounds = section_bounds(section) + item_bounds(item, item_guid) + list_bounds(list_name)
        self.project_modify(object_info, template, bounds, add)
   
   
    def scheme_modify(self, object_info: fl_object, add: bool):
    
        # Add or remove templated contents from the Xcode scheme

        contents = file_util.templated_string(fl_paths().xc_template("xcscheme"), object_info)
        self.scheme.data = file_util.modify(self.scheme.data, contents, scheme_bounds(), add)

    
    def update(self, object_info: fl_object, add: bool):

        source_exists = fl_paths().object_source_exists(object_info)
        header_exists = fl_paths().object_header_exists(object_info)
        external_object = object_info.object_file_external()
        category = object_info["category"]
        
        # Modify file items
        
        self.project_modify_section(object_info, "file_class", "PBXBuildFile", add)
        
        if source_exists:
            self.project_modify_section(object_info, "file_object", "PBXBuildFile", add)
            
        if source_exists and not external_object:
            self.project_modify_section(object_info, "file_object_for_lib", "PBXBuildFile", add)
        
        self.project_modify_section(object_info, "file_lib", "PBXBuildFile", add)
        
        # Modify file reference items

        self.project_modify_section(object_info, "fileref_product", "PBXFileReference", add)
        self.project_modify_section(object_info, "fileref_class", "PBXFileReference", add)
        
        if source_exists and not external_object:
            self.project_modify_section(object_info, "fileref_object", "PBXFileReference", add)
    
        if header_exists and not external_object:
            self.project_modify_section(object_info, "fileref_header", "PBXFileReference", add)
        
        # Modify item proxies

        self.project_modify_section(object_info, "proxy_lib", "PBXContainerItemProxy", add)
        self.project_modify_section(object_info, "proxy_target", "PBXContainerItemProxy", add)

        # Determine the correct source phase template
        
        if source_exists:
            sources_phase = "phase_sources"
        else:
            sources_phase = "phase_sources_single"
            
        # Modify additional source file items if required and futher change the source phase template
        
        if object_info["xc_obj_file_fft_guid"] != "":
            self.project_modify_section(object_info, "file_fft", "PBXBuildFile", add)
            sources_phase = "phase_sources_fft"
        elif object_info["xc_obj_file_ibuffer_guid"] != "":
            self.project_modify_section(object_info, "file_ibuffer", "PBXBuildFile", add)
            sources_phase = "phase_sources_ibuffer"

        # Modify source and framework phases for the target
        
        self.project_modify_section(object_info, sources_phase, "PBXSourcesBuildPhase", add)
        self.project_modify_section(object_info, "phase_frameworks", "PBXFrameworksBuildPhase", add)

        # Modify the target
        
        self.project_modify_section(object_info, "target", "PBXNativeTarget", add)

        # Modify dependencies
        
        self.project_modify_section(object_info, "dependency_target", "PBXTargetDependency", add)
        self.project_modify_section(object_info, "dependency_package", "PBXTargetDependency", add)

        # Modify the build configs and config list for the target
        
        self.project_modify_section(object_info, "build_config_develop", "XCBuildConfiguration", add)
        self.project_modify_section(object_info, "build_config_deploy", "XCBuildConfiguration", add)
        self.project_modify_section(object_info, "build_config_test", "XCBuildConfiguration", add)

        self.project_modify_section(object_info, "config_list", "XCConfigurationList", add)
        
        # Modify references
        
        self.project_modify_nested(object_info, "ref_dep_package", "PBXAggregateTarget", "framelib_max_package", "dependencies", add)
        self.project_modify(object_info, "ref_target", section_bounds("PBXProject") + list_bounds("targets"), add)
        self.project_modify_nested(object_info, "group_item_product", "PBXGroup", "Products", "children", add)
        
        if source_exists and not external_object:
            lib_sources_guid = object_info["xc_lib_sources_guid"]
            self.project_modify_nested(object_info, "ref_object_for_lib", "PBXSourcesBuildPhase", "Sources", "files", add, lib_sources_guid)
        
        # Get the category guids
        
        object_guid = object_info["xc_group_object_guid"]
        max_guid = object_info["xc_group_max_guid"]

        # Make the object category group if it doesn't exist

        if add and object_guid == "" and (header_exists or source_exists):

            object_guid = object_info.add_xc_object_group()
            self.project_modify(object_info, "group_object_group", section_bounds("PBXGroup"), add)
            self.project_modify_nested(object_info, "ref_object_group", "PBXGroup", "Objects FrameLib", "children", add)
            self.project_category_sort("PBXGroup", "Objects FrameLib", "children")
            
        # Make the max category group if it doesn't exist

        if add and max_guid == "":

            max_guid = object_info.add_xc_max_group()
            self.project_modify(object_info, "group_max_group", section_bounds("PBXGroup"), add)
            self.project_modify_nested(object_info, "ref_max_group", "PBXGroup", "Objects Max", "children", add)
            self.project_category_sort("PBXGroup", "Objects Max", "children")

        # Modify files in groups
        
        if header_exists and not external_object:
            self.project_modify_nested(object_info, "group_item_header", "PBXGroup", category, "children", add, object_guid)
       
        if source_exists and not external_object:
            self.project_modify_nested(object_info, "group_item_object", "PBXGroup", category, "children", add, object_guid)
        
        self.project_modify_nested(object_info, "group_item_class", "PBXGroup", category, "children", add, max_guid)
        
        # Modify the scheme for the relevant target
        
        self.scheme_modify(object_info, add)
        
        # Flush to disk
        
        self.pbxproj.flush()
        self.scheme.flush()
