
from . import file_util
import uuid

def get_guid_regex(path: str, exp: str):
    return file_util.item_regex(path, exp)

def get_vs_guid(path: str, item: str):
    return get_guid_regex(path, "\"" + item + "\".*\{(.*)\}")

def get_xcode_guid(path: str, section: str, item: str):
    return get_guid_regex(path, "([^\s].*) /\* " + item + " \*/ =")

def create_xcode_guid():
    return ''.join(str(uuid.uuid4()).upper().split('-')[1:])
    
