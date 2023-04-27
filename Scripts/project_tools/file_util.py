
from pathlib import Path
from . object_info import fl_object

    
# Class for file reading and writing

class rw_file:

    def __init__(self, path: str):
    
        self.path = path
        
        with open(path, "r") as f:
            self.data = f.read()
            
    def flush(self):
        
         with open(self.path, "w", newline = newline_setting(self.path)) as f:
            f.write(self.data)
        
        
# Determine the correct newline setting for file writing

def newline_setting(path: str):
    
    # Use window line endings for vcxproj files
        
    if Path(path).suffix == ".vcxproj":
        return '\r\n'
        
    return None
    
    
# Apply a regex search to a string (returning the first capture group)

def regex_search(data: str, exp: str):

    import re
    
    match = re.search(exp, data)
        
    if match is not None:
        return match.group(1)

    return ""


# Apply a regex search to a string using a hint to speed up the search

def regex_search_with_hint(data: str, exp: str, hint: str):

    hi = data.find(hint)
    hi = data.find("\n", hi)
    lo = data.rfind("\n", 0, hi)
    
    return regex_search(data[lo:hi], exp)


# Apply a regex search to a section of a string, with an optional hint to speed up the search

def regex_search_section(data: str, bounds: list, exp: str, hint: str = ""):
    
    index_lo, index_hi = find_section(data, bounds)
    data = data[index_lo:index_hi]
    
    if hint != "":
        return regex_search_with_hint(data, exp, hint)
    else:
        return regex_search(data, exp)


# Find a section of a string based on a list of pairs of bounds

def find_section(data: str, bounds: list):

    index_lo = 0
    index_hi = len(data)
    
    for idx in range(int(len(bounds) / 2)):
        index_lo = data.find(bounds[idx * 2 + 0], index_lo, index_hi)
        index_hi = data.find(bounds[idx * 2 + 1], index_lo + len(bounds[idx * 2 + 0]), index_hi)

    if len(bounds) % 2:
        index_lo = data.find(bounds[len(bounds) - 1], index_lo, index_hi)

    return index_lo, index_hi
    
    
# Find the next blank line in a string given an index to start from

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
        
        
# Insert one string into another with a section specified by bounds (with an optional

def insert(data: str, contents: str, bounds: list, next_blank: bool = False):
    
    index_lo, index = find_section(data, bounds)
                
    # Look for next whitespace line (skipping the first which will be immediate)
        
    if next_blank:
        index = find_next_blankline(data, index)
            
    return data[:index] + contents + data[index:]


# Remove one string from another within a section specified with bounds

def remove(data: str, contents: str, bounds: list):
        
    index_lo, index_hi = find_section(data, bounds)
    index = data.find(contents, index_lo, index_hi)
        
    if index < 0:
        print("\nWARNING - FAILED TO REMOVE\n")
        print(contents)
        return data
        
    return data[:index] + data[index + len(contents):]
    
    
# Insert or remove one string from another within a section specified with bounds

def modify(data: str, contents: str, bounds: list, add: bool):
    if add:
        return insert(data, contents, bounds)
    else:
        return remove(data, contents, bounds)


# Sort lines in a specificed section of a string using a regex to select the part of each line to sort on

def sort_section(data: str, bounds: list, exp: str):
    
    lo, hi = find_section(data, bounds)

    lines = data[lo:hi].splitlines(True)
    lines.sort(key = lambda a : regex_search(a.upper(), exp))
    return data[:lo] + "".join(lines) + data[hi:]


# Search a string for the next tag that need replacing and then replace all instances of the given key

def replace_next_key(data: str, info: fl_object):

    lo, hi = find_section(data, ["_##", "##_"])
    
    if lo < 0 or hi < 0:
        return False, data

    key = data[lo+3:hi]
    value = info[key.lower()]
            
    return True, data.replace("_##" + key + "##_", value)
    
    
# Return a string based on a template, but with all tagged items replaced

def templated_string(template_path: str, info: fl_object):

    with open(template_path, "r") as f:
        template = f.read()

    condition = True;
    
    while condition:
        condition, template = replace_next_key(template, info)
        
    return template
    

# Create a file based on a template with an option as to whether pre-existing files should be overwritten

def create(output_path: str, template_path: str, info: fl_object, overwrite: bool = False):

    contents = templated_string(template_path, info)
    
    if overwrite:
        mode = "w"
    else:
        mode = "x"
        
    f = open(output_path, mode, newline = newline_setting(output_path))
    f.write(contents)
    f.close()
      
    
# Create a file based on a template with an option as to whether pre-existing files should be overwritten

def insert_file(path: str, contents: str, bounds: list, next_blank: bool = False):
    
    file = rw_file(path)
    file.data = insert(file.data, contents, bounds, next_blank)
    file.flush()
