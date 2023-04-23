
from . path_util import fl_paths
from . import file_util

import uuid


def get_xcode_path():
    return fl_paths().xcode_pbxproj()
    

def get_vs_guid(item: str):
    return file_util.item_regex(fl_paths().vs_solution(), "\"" + item + "\".*\{(.*)\}")
    
    
def get_xcode_guid(section: str, item: str):

    from . xcode_util import section_bounds
     
    bounds = section_bounds(section)
    return file_util.section_regex(get_xcode_path(), bounds, "([^\s]+) /\* " + item + " \*/ = \{")


def get_xcode_component_guid(section: str, item: str, list: str, component: str, guid: str = None):
    
    from . xcode_util import section_bounds
    from . xcode_util import item_bounds
    from . xcode_util import list_bounds
     
    bounds = section_bounds(section) + item_bounds(item, guid) + list_bounds(list)
    return file_util.section_regex(get_xcode_path(), bounds, "([^\s]+) /\* " + component + " \*/")
    
    
def get_xcode_field_guid(section: str, guid: str, field: str):
    
    from . xcode_util import section_bounds
    from . xcode_util import item_bounds
     
    bounds = section_bounds(section) + item_bounds(section, guid)
    return file_util.section_regex(get_xcode_path(), bounds, field + " = ([^\s]+) /\*.*?\*/")
    
    
def guid_check_duplicate(path: str, guid: str):
    return file_util.item_regex(path, "(" + guid + ")") != ""


def create_vs_guid():
    guid = str(uuid.uuid4()).upper()
        
    # Ensure that the GUID is not in use already
        
    if guid_check_duplicate(fl_paths().vs_solution(), guid):
        return create_vs_guid()
        
    return guid
    
    
def create_xcode_guid():
    return ''.join(str(uuid.uuid4()).upper().split('-')[1:])
    
