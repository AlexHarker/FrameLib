from shutil import copyfile
from framelib.utils import write_json, read_json
from framelib.classes import Documentation


def replace_jsarguments(box: dict, obj_name: str) -> None:
    if "jsarguments" in box and (box["filename"] == "fl.helpdetails.js" or box["filename"] == "fl.helpname.js"):
            box["jsarguments"] = obj_name

def rename_help(docs: Documentation, file_edit: str, obj_name: str) -> None:
    """Takes a path to a Max patch and does a find and replace on object names"""
    obj_name = docs.refpage_name(obj_name)
    d = read_json(file_edit)
    
    outer_boxes = d["patcher"]["boxes"]

    # Iterate over tabs
    
    for box in outer_boxes:
        inner_boxes = box["box"]["patcher"]["boxes"]
        for child_box in inner_boxes:
            replace_jsarguments(child_box["box"], obj_name)

    write_json(file_edit, d)
    
def resize_patch(patch: dict, width: float, height: float) -> None:
    """Takes a patcher as a dict and resizes it"""
    patch["rect"][2] = width
    patch["rect"][3] = height

def resize_help(docs: Documentation, file_edit: str, width: float, height: float) -> None:
    """Takes a path to a Max patch and resizes the window for the main patch and all tabs"""
    d = read_json(file_edit)
    
    outer_patch = d["patcher"]
    resize_patch(outer_patch, width, height)
    outer_boxes = outer_patch["boxes"]
    
    # Iterate over tabs
    
    for box in outer_boxes:
        inner_patch = box["box"]["patcher"]
        resize_patch(inner_patch, width, height - 26)

    write_json(file_edit, d)
