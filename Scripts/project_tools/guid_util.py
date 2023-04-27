
from . path_util import fl_paths
from . file_util import rw_file
from . file_util import regex_search
from . file_util import regex_search_section
from . xc_util import section_bounds
from . xc_util import item_bounds
from . xc_util import list_bounds

import uuid


class guid_manager:

    def __init__(self):
    
        self.pbxproj = rw_file(fl_paths().xc_pbxproj())
        self.solution = rw_file(fl_paths().vs_solution())
       
       
    def vs(self, item: str):
        return regex_search(self.solution.data, "\"" + item + "\".*\{(.*)\}")
        
        
    def vs_main(self):
        return regex_search(self.solution.data, "Project\(\"\{(.*)\}\"\) = \"framelib\"")

        
    def xc(self, section: str, item: str):
         
        bounds = section_bounds(section)
        exp = "([^\s]+) /\* " + item + " \*/ = \{"
        hint = "/* " + item + " */"
        return regex_search_section(self.pbxproj.data, bounds, exp, hint)


    def xc_component(self, section: str, item: str, list: str, component: str, guid: str = None):
        
        bounds = section_bounds(section) + item_bounds(item, guid) + list_bounds(list)
        exp = "([^\s]+) /\* " + component + " \*/"
        hint = "/* " + component + " */"
        return regex_search_section(self.pbxproj.data, bounds, exp, hint)
        
        
    def xc_field(self, section: str, guid: str, field: str):
         
        bounds = section_bounds(section) + item_bounds(section, guid)
        exp = field + " = ([^\s]+) /\*.*?\*/"
        hint = field + " = "
        return regex_search_section(self.pbxproj.data, bounds, exp, hint)
        
    
    def xc_target_dependency(self, guid: str):
                        
        bounds = section_bounds("PBXTargetDependency") + ["/* Begin", "target = " + guid]
        exp = "[\S\s]*\s([^\s]+) /\* PBXTargetDependency \*/ = \{[\S\s]*?\Z"
        return regex_search_section(self.pbxproj.data, bounds, exp)
        
        
    # Ensure that the GUID is not in use already

    def __check_duplicate(self, data: str, guid: str):
        return regex_search(data, "(" + guid + ")") != ""


    def create_vs(self):

        guid = str(uuid.uuid4()).upper()
            
        if self.__check_duplicate(self.solution.data, guid):
            return create_vs()
            
        return guid
        
        
    def create_xc(self):
        
        guid = ''.join(str(uuid.uuid4()).upper().split('-')[1:])
        
        if self.__check_duplicate(self.pbxproj.data, guid):
            return create_xc()
            
        return guid
