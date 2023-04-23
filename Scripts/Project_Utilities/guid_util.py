
from . path_util import fl_paths
from . import file_util

import uuid

def get_guid_regex(path: str, exp: str):
    return file_util.item_regex(path, exp)

def get_vs_guid(path: str, item: str):
    return get_guid_regex(path, "\"" + item + "\".*\{(.*)\}")

def get_xcode_guid(path: str, section: str, item: str):
    return get_guid_regex(path, "([^\s].*) /\* " + item + " \*/ =")

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
    
