
import re
import uuid

from pathlib import Path
from . object_info import fl_object

    
def item_regex(path: str, exp: str):

    regex = re.compile(exp)
    
    with open(path) as f:
        data = f.read()
        match = regex.search(data)
        
        if match is not None:
            return match.group(1)

    return ""
    
    
def get_guid_regex(path: str, exp: str):
    return item_regex(path, exp)


def get_vs_guid(path: str, item: str):
    return get_guid_regex(path, "\"" + item + "\".*\{(.*)\}")


def get_xcode_guid(path: str, item: str):
    return get_guid_regex(path, "([^\s].*) /\* " + item + " \*/ =")


def create_xcode_guid():
    return ''.join(str(uuid.uuid4()).upper().split('-')[1:])
    
    
def lines_regex(path: str, exp: str, start: str, end: str, inner_start: str, inner_end: str):
    
    regex = re.compile(exp)
    list = []
    started1 = False
    started2 = False
    started = False
    
    with open(path) as f:
        for line in f:
            match = regex.search(line)
            
            if start in line:
                started1 = True
            if inner_start in line:
                started2 = True
              
            if started and (end in line or inner_end in line):
                return list
                
            started = started1 and started2
            
            if started and match is not None:
                list.append([match.group(1), match.group(0)])

    return list
    
    
def templated_string(template_path: str, object_info: fl_object):

    f = open(template_path, "r")
    template = f.read()
    f.close()

    template = template.replace("_##CLASS##_", object_info.object_class)
    template = template.replace("_##CLASS_UPPER##_", object_info.object_class.upper())
    template = template.replace("_##CLASSNAME##_", object_info.max_class_name)
    template = template.replace("_##CATEGORY##_", object_info.category)
    template = template.replace("_##GUID##_", object_info.guid)
    
    template = template.replace("_##VS_FRAMELIB_GUID##_", object_info.vs_fl_guid)
    template = template.replace("_##VS_FRAMELIB_OBJ_GUID##_", object_info.vs_fl_objects_guid)
    template = template.replace("_##VS_MAX_OBJECTS_GUID##_", object_info.vs_fl_max_objects_guid)
    template = template.replace("_##VS_MAIN_GUID##_", object_info.vs_main_guid)
    
    template = template.replace("_##XCODE_MAIN_GUID##_", object_info.xcode_main_guid)
    template = template.replace("_##XCODE_FRAMELIB_GUID##_", object_info.xcode_framelib_guid)
    template = template.replace("_##XCODE_MAX_CONFIG_GUID##_", object_info.xcode_max_config_guid)
    template = template.replace("_##XCODE_FILEREF_LIB_GUID##_", object_info.xcode_fileref_lib_guid)

    template = template.replace("_##XCODE_OBJ_TARGET_GUID##_", object_info.xcode_obj_target_guid)
    template = template.replace("_##XCODE_OBJ_PACKAGE_DEP_GUID##_", object_info.xcode_obj_package_dep_guid)
    template = template.replace("_##XCODE_OBJ_LIB_DEP_GUID##_", object_info.xcode_obj_lib_dep_guid)

    template = template.replace("_##XCODE_OBJ_LIB_PROXY_GUID##_", object_info.xcode_obj_lib_proxy_guid)
    template = template.replace("_##XCODE_OBJ_TARGET_PROXY_GUID##_", object_info.xcode_obj_target_proxy_guid)
    
    template = template.replace("_##XCODE_OBJ_SOURCES_GUID##_", object_info.xcode_obj_sources_guid)
    template = template.replace("_##XCODE_OBJ_FRAMEWORKS_GUID##_", object_info.xcode_obj_frameworks_guid)

    template = template.replace("_##XCODE_OBJ_FILE_CLASS_GUID##_", object_info.xcode_obj_file_class_guid)
    template = template.replace("_##XCODE_OBJ_FILEREF_CLASS_GUID##_", object_info.xcode_obj_fileref_class_guid)
    template = template.replace("_##XCODE_OBJ_FILE_OBJECT_GUID##_", object_info.xcode_obj_file_object_guid)
    template = template.replace("_##XCODE_OBJ_FILEREF_OBJECT_GUID##_", object_info.xcode_obj_fileref_object_guid)
    template = template.replace("_##XCODE_OBJ_FILE_LIB_GUID##_", object_info.xcode_obj_file_lib_guid)
    template = template.replace("_##XCODE_OBJ_FILEREF_MXO_GUID##_", object_info.xcode_obj_fileref_mxo_guid)
    
    template = template.replace("_##XCODE_OBJ_CONFIG_LIST_GUID##_", object_info.xcode_obj_config_list_guid)
    template = template.replace("_##XCODE_OBJ_CONFIG_DVMT_GUID##_", object_info.xcode_obj_config_dvmt_guid)
    template = template.replace("_##XCODE_OBJ_CONFIG_DPLT_GUID##_", object_info.xcode_obj_config_dplt_guid)
    template = template.replace("_##XCODE_OBJ_CONFIG_TEST_GUID##_", object_info.xcode_obj_config_test_guid)

    return template
    

def create(output_path: str, template_path: str, object_info: fl_object):

    contents = templated_string(template_path, object_info)
    
    # Use window line endings for vcxproj files
    
    posix_path = Path(output_path)
    nl = None;
    
    if posix_path.suffix == ".vcxproj":
        nl = '\r\n'
        
    f = open(output_path, "w", newline=nl)
    f.write(contents)
    f.close()


def find_next_blankline(data: str, index: int):
    while True:
        next = data.find("\n", index)
        if next >= 0:
            next = data.find("\n", next + 1)
        if data[index:next].isspace():
            return index + 1
        if next < 0:
            return index
        index = next
            
            
def insert(path: str, contents: str, start: str, end: str, next_blank: bool = False):
    
    data = ""
    
    with open(path, "r") as f:
        data = f.read()
        index = data.find(start) + len(start)
        index = data.find(end, index)
        
        # Look for next whitespace line (skipping the first which will be immediate)
        
        if next_blank:
            index = find_next_blankline(data, index)
            
    with open(path, "w") as f:
        f.write(data[:index] + contents + data[index:])


def remove(path: str, contents: str, start: str, end: str):
    
    data = ""
    
    with open(path, "r") as f:
        data = f.read()
        index_start = data.find(start)
        index_end = data.find(end, index_start + len(start))
        index = data.find(contents, index_start, index_end)
        
    if index < 0:
        return
        
    with open(path, "w") as f:
        f.write(data[:index] + data[index + len(contents):])


def insert_remove(path:str, contents: str, start: str, end: str, add: bool):
    if add:
        insert(path, contents, start, end)
    else:
        remove(path, contents, start, end)
