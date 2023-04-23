
from . import file_util
from . path_util import fl_paths
from . object_info import fl_object


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


def project_modify(object_info: fl_object, template: str, bounds: list, add: bool):
    
    contents = file_util.templated_string(fl_paths().template("xcode_templates/" + template), object_info)
    file_util.modify(fl_paths().xcode_pbxproj(), contents, bounds, add)
      
      
def project_modify_section(object_info: fl_object, template: str, section: str, add: bool):
    project_modify(object_info, template, section_bounds(section), add)
       
    
def scheme_modify(object_info: fl_object, add: bool):
    
    contents = file_util.templated_string(fl_paths().template("xcode_templates/xcscheme"), object_info)
    file_util.modify(fl_paths().xcode_scheme(), contents, scheme_bounds(), add)
    

class fl_pbxproj:

    def update(self, object_info: fl_object, add: bool):

        project_modify_section(object_info, "file_class", "PBXBuildFile", add)
        
        if fl_paths().object_source_exists(object_info):
            project_modify_section(object_info, "file_object", "PBXBuildFile", add)
            project_modify_section(object_info, "file_object_for_lib", "PBXBuildFile", add)
        
        project_modify_section(object_info, "file_lib", "PBXBuildFile", add)
        
        project_modify_section(object_info, "fileref_product", "PBXFileReference", add)
        project_modify_section(object_info, "fileref_class", "PBXFileReference", add)
        
        if fl_paths().object_source_exists(object_info):
            project_modify_section(object_info, "fileref_object", "PBXFileReference", add)
    
        if fl_paths().object_header_exists(object_info):
            project_modify_section(object_info, "fileref_header", "PBXFileReference", add)
        
        project_modify_section(object_info, "proxy_lib", "PBXContainerItemProxy", add)
        project_modify_section(object_info, "proxy_target", "PBXContainerItemProxy", add)

        if fl_paths().object_source_exists(object_info):
            sources_phase = "phase_sources"
        else:
            sources_phase = "phase_sources_single"
            
        if object_info.xcode_obj_file_fft_guid != "":
            project_modify_section(object_info, "file_fft", "PBXBuildFile", add)
            sources_phase = "phase_sources_fft"
        elif object_info.xcode_obj_file_ibuffer_guid != "":
            project_modify_section(object_info, "file_ibuffer", "PBXBuildFile", add)
            sources_phase = "phase_sources_ibuffer"

        project_modify_section(object_info, sources_phase, "PBXSourcesBuildPhase", add)
        project_modify_section(object_info, "phase_frameworks", "PBXFrameworksBuildPhase", add)

        project_modify_section(object_info, "target", "PBXNativeTarget", add)

        project_modify_section(object_info, "dependency_target", "PBXTargetDependency", add)
        project_modify_section(object_info, "dependency_package", "PBXTargetDependency", add)

        project_modify_section(object_info, "build_config_develop", "XCBuildConfiguration", add)
        project_modify_section(object_info, "build_config_deploy", "XCBuildConfiguration", add)
        project_modify_section(object_info, "build_config_test", "XCBuildConfiguration", add)

        project_modify_section(object_info, "config_list", "XCConfigurationList", add)
        
        bounds = section_bounds("PBXAggregateTarget") + item_bounds("framelib_max_package") + list_bounds("dependencies")
        project_modify(object_info, "ref_dep_package", bounds, add)
        
        bounds = section_bounds("PBXProject") + list_bounds("targets")
        project_modify(object_info, "ref_target", bounds, add)
        
        if fl_paths().object_source_exists(object_info):
            bounds = section_bounds("PBXSourcesBuildPhase") + item_bounds("Sources", object_info.xcode_lib_sources_guid) + list_bounds("files")
            project_modify(object_info, "ref_object_for_lib", bounds, add)
        
        bounds = section_bounds("PBXGroup") + item_bounds("Products") + list_bounds("children")
        project_modify(object_info, "group_item_product", bounds, add)
        
        # FIX - we assume that the categories exist - so we'll need to make them if not
        
        exp = "([^\s].*) /\* " + object_info.category + " \*/"
        
        bounds = section_bounds("PBXGroup") + item_bounds("Objects FrameLib") + list_bounds("children")
        object_guid = file_util.section_regex(fl_paths().xcode_pbxproj(), bounds, exp)
        
        bounds = section_bounds("PBXGroup") + item_bounds("Objects Max") + list_bounds("children")
        max_guid = file_util.section_regex(fl_paths().xcode_pbxproj(), bounds, exp)
        
        if fl_paths().object_source_exists(object_info):
            bounds = section_bounds("PBXGroup") + item_bounds(object_info.category, object_guid) + list_bounds("children")
            project_modify(object_info, "group_item_object", bounds, add)
        
        if fl_paths().object_header_exists(object_info):
            bounds = section_bounds("PBXGroup") + item_bounds(object_info.category, object_guid) + list_bounds("children")
            project_modify(object_info, "group_item_header", bounds, add)
        
        bounds = section_bounds("PBXGroup") + item_bounds(object_info.category, max_guid) + list_bounds("children")
        project_modify(object_info, "group_item_class", bounds, add)
        
        scheme_modify(object_info, add)
