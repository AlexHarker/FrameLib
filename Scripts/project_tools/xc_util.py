
from . path_util import fl_paths
from . object_info import fl_object

from . import file_util


# Helpers to generate bounds for sections / items / lists in the Xcode project

def section_bounds(section: str):
    return ["/* Begin " + section + " section */\n", "/* End " + section + " section */"]


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
       
       
    def project_modify(self, info: fl_object, template: str, bounds: list, add: bool):
    
        # Add or remove templated contents from the Xcode project
        
        contents = file_util.templated_string(fl_paths().xc_template(template), info)
        self.pbxproj.data = file_util.modify(self.pbxproj.data, contents, bounds, add)
    
    
    def project_category_sort(self, section: str, item: str, list_name: str):
    
        # Sort an Xcode project category folder

        bounds = section_bounds(section) + item_bounds(item) + list_bounds(list_name) + ["Complex_Binary"]
        self.pbxproj.data = file_util.sort_section(self.pbxproj.data, bounds, "/\*(.*)\*/")
        
      
    def project_modify_section(self, info: fl_object, template: str, section: str, add: bool):
    
        # Modify a named section of the Xcode project
            
        self.project_modify(info, template, section_bounds(section), add)
        
    
    def project_modify_list(self, info: fl_object, template: str, section: str, item: str, list_name: str, add: bool, item_guid: str = None):
    
        # Modify a nested list in the Xcode project
            
        bounds = section_bounds(section) + item_bounds(item, item_guid) + list_bounds(list_name)
        self.project_modify(info, template, bounds, add)
   
   
    def scheme_modify(self, info: fl_object, add: bool):
    
        # Add or remove templated contents from the Xcode scheme

        contents = file_util.templated_string(fl_paths().xc_template("xcscheme"), info)
        self.scheme.data = file_util.modify(self.scheme.data, contents, scheme_bounds(), add)

    
    def update(self, info: fl_object, add: bool):

        source_exists = fl_paths().object_source_exists(info)
        header_exists = fl_paths().object_header_exists(info)
        external_object = info.object_file_external()
        category = info["category"]
        
        # Modify file items
        
        self.project_modify_section(info, "file_class", "PBXBuildFile", add)
        
        if source_exists:
            self.project_modify_section(info, "file_object", "PBXBuildFile", add)
            
        if source_exists and not external_object:
            self.project_modify_section(info, "file_object_for_lib", "PBXBuildFile", add)
        
        self.project_modify_section(info, "file_lib", "PBXBuildFile", add)
        
        # Modify file reference items

        self.project_modify_section(info, "fileref_product", "PBXFileReference", add)
        self.project_modify_section(info, "fileref_class", "PBXFileReference", add)
        
        if source_exists and not external_object:
            self.project_modify_section(info, "fileref_object", "PBXFileReference", add)
    
        if header_exists and not external_object:
            self.project_modify_section(info, "fileref_header", "PBXFileReference", add)
        
        # Modify item proxies

        self.project_modify_section(info, "proxy_lib", "PBXContainerItemProxy", add)
        self.project_modify_section(info, "proxy_target", "PBXContainerItemProxy", add)

        # Determine the correct source phase template
        
        if source_exists:
            sources_phase = "phase_sources"
        else:
            sources_phase = "phase_sources_single"
            
        # Modify additional source file items if required and futher change the source phase template
        
        if info["xc_obj_file_ibuffer_guid"] != "":
            self.project_modify_section(info, "file_ibuffer", "PBXBuildFile", add)
            sources_phase = "phase_sources_ibuffer"

        # Modify source and framework phases for the target
        
        self.project_modify_section(info, sources_phase, "PBXSourcesBuildPhase", add)
        self.project_modify_section(info, "phase_frameworks", "PBXFrameworksBuildPhase", add)

        # Modify the target
        
        self.project_modify_section(info, "target", "PBXNativeTarget", add)

        # Modify dependencies
        
        self.project_modify_section(info, "dependency_target", "PBXTargetDependency", add)
        self.project_modify_section(info, "dependency_package", "PBXTargetDependency", add)

        # Modify the build configs and config list for the target
        
        self.project_modify_section(info, "build_config_develop", "XCBuildConfiguration", add)
        self.project_modify_section(info, "build_config_deploy", "XCBuildConfiguration", add)
        self.project_modify_section(info, "build_config_test", "XCBuildConfiguration", add)

        self.project_modify_section(info, "config_list", "XCConfigurationList", add)
        
        # Modify references
        
        self.project_modify_list(info, "ref_dep_package", "PBXAggregateTarget", "framelib_max_package", "dependencies", add)
        self.project_modify(info, "ref_target", section_bounds("PBXProject") + list_bounds("targets"), add)
        self.project_modify_list(info, "group_item_product", "PBXGroup", "Products", "children", add)
        
        if source_exists and not external_object:
            guid = info["xc_lib_sources_guid"]
            self.project_modify_list(info, "ref_object_for_lib", "PBXSourcesBuildPhase", "Sources", "files", add, guid)
        
        # Get the category guids
        
        object_guid = info["xc_group_object_guid"]
        max_guid = info["xc_group_max_guid"]

        # Make the object category group if it doesn't exist

        if add and object_guid == "" and (header_exists or source_exists):

            object_guid = info.add_xc_object_group()
            self.project_modify_section(info, "group_object_group", "PBXGroup", add)
            self.project_modify_list(info, "ref_object_group", "PBXGroup", "Objects FrameLib", "children", add)
            self.project_category_sort("PBXGroup", "Objects FrameLib", "children")
            
        # Make the max category group if it doesn't exist

        if add and max_guid == "":

            max_guid = info.add_xc_max_group()
            self.project_modify_section(info, "group_max_group", "PBXGroup", add)
            self.project_modify_list(info, "ref_max_group", "PBXGroup", "Objects Max", "children", add)
            self.project_category_sort("PBXGroup", "Objects Max", "children")

        # Modify files in groups
        
        if header_exists and not external_object:
            self.project_modify_list(info, "group_item_header", "PBXGroup", category, "children", add, object_guid)
       
        if source_exists and not external_object:
            self.project_modify_list(info, "group_item_object", "PBXGroup", category, "children", add, object_guid)
        
        self.project_modify_list(info, "group_item_class", "PBXGroup", category, "children", add, max_guid)
        
        # Modify the scheme for the relevant target
        
        self.scheme_modify(info, add)
        
        # Flush to disk
        
        self.pbxproj.flush()
        self.scheme.flush()
        

    def __sort_section(self, section: str, exp: str):
        
        bounds = section_bounds(section)
        self.pbxproj.data = file_util.sort_section(self.pbxproj.data, bounds, exp)
    
    
    def __sort_section_items(self, section: str, exp: str):
        
        bounds = section_bounds(section)
        self.pbxproj.data = file_util.sort_section_items(self.pbxproj.data, bounds, "\n\t\t};\n", exp)
        
    
    def sort_project(self):
        
        exp = "([^\s]+) /\* .+ \*/ = \{"
        
        self.__sort_section("PBXBuildFile", exp)
        self.__sort_section("PBXFileReference", exp)
        
        self.__sort_section_items("PBXContainerItemProxy", exp)
        self.__sort_section_items("PBXFrameworksBuildPhase", exp)
        self.__sort_section_items("PBXNativeTarget", exp)
        self.__sort_section_items("PBXSourcesBuildPhase", exp)
        self.__sort_section_items("PBXTargetDependency", exp)
        self.__sort_section_items("XCBuildConfiguration", exp)
        self.__sort_section_items("XCConfigurationList", exp)
        
        self.pbxproj.flush()
