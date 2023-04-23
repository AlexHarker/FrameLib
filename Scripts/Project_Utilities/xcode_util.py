
from . import file_util
from . path_util import fl_paths
from . object_info import fl_object

from pathlib import Path


def section_bounds(section: str):
        return ["/* Begin " + section + " section */", "/* End " + section + " section */"]
    
    
def project_modify(object_info: fl_object, template: str, bounds: list, add: bool):
    
    contents = file_util.templated_string(fl_paths().template("xcode_templates/" + template), object_info)
    file_util.modify(fl_paths().xcode_pbxproj(), contents, bounds, add)
      
      
def project_modify_section(object_info: fl_object, template: str, section: str, add: bool):
    project_modify(object_info, template, section_bounds(section), add)
       
    
class fl_pbxproj:

    def update(self, object_info: fl_object, add: bool):

        project_modify_section(object_info, "file_class", "PBXBuildFile", add)
        project_modify_section(object_info, "file_object", "PBXBuildFile", add)
        project_modify_section(object_info, "file_lib", "PBXBuildFile", add)
        project_modify_section(object_info, "file_object_for_lib", "PBXBuildFile", add)
        
        project_modify_section(object_info, "fileref_class", "PBXFileReference", add)
        project_modify_section(object_info, "fileref_object", "PBXFileReference", add)
        project_modify_section(object_info, "fileref_product", "PBXFileReference", add)

        project_modify_section(object_info, "proxy_lib", "PBXContainerItemProxy", add)
        project_modify_section(object_info, "proxy_target", "PBXContainerItemProxy", add)

        project_modify_section(object_info, "phase_sources", "PBXSourcesBuildPhase", add)
        project_modify_section(object_info, "phase_frameworks", "PBXFrameworksBuildPhase", add)

        project_modify_section(object_info, "target", "PBXNativeTarget", add)

        project_modify_section(object_info, "dependency_target", "PBXTargetDependency", add)
        project_modify_section(object_info, "dependency_package", "PBXTargetDependency", add)

        project_modify_section(object_info, "build_config_develop", "XCBuildConfiguration", add)
        project_modify_section(object_info, "build_config_deploy", "XCBuildConfiguration", add)
        project_modify_section(object_info, "build_config_test", "XCBuildConfiguration", add)

        project_modify_section(object_info, "config_list", "XCConfigurationList", add)
        
        bounds = section_bounds("PBXAggregateTarget") + ["/* framelib_max_package */ = {", "};", "dependencies = (", "\t\t\t);"]
        project_modify(object_info, "ref_dep_package", bounds, add)
        
        bounds = section_bounds("PBXProject") + ["targets = (", "\t\t\t);"]
        project_modify(object_info, "ref_target", bounds, add)
        
        tag = object_info.xcode_lib_sources_guid + " /* Sources */ = {"
        bounds = section_bounds("PBXSourcesBuildPhase") + [tag, "};", "files = (", "\t\t\t);"]
        project_modify(object_info, "ref_object_for_lib", bounds, add)
        
        
    def update_project(self, path: str, add: bool):

        class_name = path.rsplit("/", 1)[1].replace(".vcxproj", "")
        category = file_util.item_regex(path, "FrameLib_Max_Objects\\\\(.*)\\\\fl")
        guid = file_util.item_regex(path, "<ProjectGuid>\{(.*)\}</ProjectGuid>")

        object_info = fl_object("", class_name, category)
        object_info.guid = guid

        paths = fl_paths()
        max_object_path = paths.max_source(object_info)
        object_info.object_class = file_util.item_regex(max_object_path, "FrameLib_MaxClass_Expand<(.*)>").split(",")[0]

        if object_info.object_class == "":
            object_info.object_class = file_util.item_regex(max_object_path, "FrameLib_MaxClass<(.*)>")

        if object_info.object_class == "":
            object_info.object_class = file_util.item_regex(max_object_path, "FrameLib_MaxClass_ExprParsed<(.*)>")
           
        self.update(object_info, add)
        
        file_util.create(paths.vs_max_project(object_info), paths.template("fl.class_name~.vcxproj"), object_info)


    def update_all_projects(self, add: bool):

        projects = Path(fl_paths().vs_max_projects()).glob('fl.*.vcxproj')
        project_list = list(projects)
        project_list.sort()
        
        for project in project_list:
            print(project)
            self.update_project(project.as_posix(), add)
