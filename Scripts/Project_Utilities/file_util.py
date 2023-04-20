
import re

def item_regex(path: str, exp: str):

    regex = re.compile(exp)
    
    with open(path) as f:
        data = f.read()
        match = regex.search(data)
        
        if match is not None:
            return match.group(1)

    return ""
    

def lines_regex(path: str, exp: str, start: str, end: str):
    
    regex = re.compile(exp)
    list = []
    started = False;
    
    with open(path) as f:
        for line in f:
            match = regex.search(line)
            
            if start in line:
                started = True
                
            if started and end in line:
                return list
            
            if started and match is not None:
                list.append(match.group(1))

    return list
    
    
def create(template_path: str, output_path: str, object_class: str, class_name: str, category: str, guid: str):

    f = open(template_path, "r")
    template = f.read()
    f.close()

    template = template.replace("_##CLASS##_", object_class)
    template = template.replace("_##CLASS_UPPER##_", object_class.upper())
    template = template.replace("_##CLASSNAME##_", class_name)
    template = template.replace("_##CATEGORY##_", category)
    template = template.replace("_##GUID##_", guid)
    
    f = open(output_path, "x")
    f.write(template)
    f.close()


def insert(path: str, contents: str, start: str, end: str):
    
    data = ""
    
    with open(path, "r") as f:
        data = f.read()
        index = data.find(start) + len(start)
        index = data.find(end, index)
        
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
