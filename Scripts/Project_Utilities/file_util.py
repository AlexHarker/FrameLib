
from pathlib import Path
from . object_info import fl_object


def newline_setting(path: str):
    
    # Use window line endings for vcxproj files
        
    if Path(path).suffix == ".vcxproj":
        return '\r\n'
        
    return None
    
    
class rw_file:

    def __init__(self, path: str):
    
        self.path = path
        
        with open(path, "r") as f:
            self.data = f.read()
            
    def flush(self):
        
         with open(self.path, "w", newline = newline_setting(self.path)) as f:
            f.write(self.data)
        
            

def do_regex(data: str, exp: str):

    import re
    
    regex = re.compile(exp)
    match = regex.search(data)
        
    if match is not None:
        return match.group(1)

    return ""


def do_regex_with_hint(data: str, exp: str, hint: str):

    hi = data.find(hint)
    hi = data.find("\n", hi)
    lo = data.rfind("\n", 0, hi)
    
    return do_regex(data[lo:hi], exp)

    
def lines_regex(path: str, exp: str, start: str, end: str, inner_start: str, inner_end: str):
    
    import re
    
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
    
    
def create(output_path: str, template_path: str, object_info: fl_object, overwrite: bool = False):

    contents = templated_string(template_path, object_info)
    
    if overwrite:
        mode = "w"
    else:
        mode = "x"
        
    f = open(output_path, mode, newline = newline_setting(output_path))
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
            
            
def find_section(data: str, bounds: list):

    index_lo = 0
    index_hi = len(data)
    
    for idx in range(int(len(bounds) / 2)):
        index_lo = data.find(bounds[idx * 2 + 0], index_lo, index_hi)
        index_hi = data.find(bounds[idx * 2 + 1], index_lo + len(bounds[idx * 2 + 0]), index_hi)

    return index_lo, index_hi
    
    
def get_section_string(data: str, bounds: list):
        
    index_lo, index_hi = find_section(data, bounds)
    return data[index_lo:index_hi]
   

def section_regex_string(data: str, bounds: list, exp: str, hint: str = ""):
    
    data = get_section_string(data, bounds)
    
    if hint != "":
        return do_regex_with_hint(data, exp, hint)
    else:
        return do_regex(data, exp)
        
        
def insert_string(data: str, contents: str, bounds: list, next_blank: bool = False):
    
    index_lo, index = find_section(data, bounds)
                
    # Look for next whitespace line (skipping the first which will be immediate)
        
    if next_blank:
        index = find_next_blankline(data, index)
            
    return data[:index] + contents + data[index:]


def remove_string(data: str, contents: str, bounds: list):
        
    index_lo, index_hi = find_section(data, bounds)
    index = data.find(contents, index_lo, index_hi)
        
    if index < 0:
        print("WARNING - FAILED TO REMOVE\n")
        print(contents)
        return data
        
    return data[:index] + data[index + len(contents):]
    
    
def modify_string(data: str, contents: str, bounds: list, add: bool):
    if add:
        return insert_string(data, contents, bounds)
    else:
        return remove_string(data, contents, bounds)
        
       
def item_regex(path: str, exp: str):
    
    file = rw_file(path)
    return do_regex(file.data, exp)
      
        
def insert(path: str, contents: str, bounds: list, next_blank: bool = False):
    
    file = rw_file(path)
    file.data = insert_string(file.data, contents, bounds, next_blank)
    file.flush()


def replace_next_key(data: str, object_info: fl_object):

    lo, hi = find_section(data, ["_##", "##_"])
    
    if lo < 0 or hi < 0:
        return False, data

    key = data[lo+3:hi]
    
    if key.lower() in object_info.__dict__:
        value = object_info.__dict__[key.lower()]
    else:
        value = fl_object.__dict__[key.lower()]
            
    return True, data.replace("_##" + key + "##_", value)
    
    
def templated_string(template_path: str, object_info: fl_object):

    with open(template_path, "r") as f:
        template = f.read()

    condition = True;
    
    while condition:
        condition, template = replace_next_key(template, object_info)
        
    return template
