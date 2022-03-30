from shutil import copyfile
from framelib.utils import write_json, read_json
from framelib.aliases import refpage_name


def replace_jsarguments(box: dict, obj_name: str) -> None:
    if "jsarguments" in box and (box["filename"] == "fl.helpdetails.js" or box["filename"] == "fl.helpname.js"):
            box["jsarguments"] = obj_name

def edit_help(docs: str, file_edit: str, obj_name: str) -> None:
    """Takes a path to a Max patch and does a find and replace on object names"""
    #obj_name = refpage_name(docs, obj_name)
    d = read_json(file_edit)
    
    outer_boxes = d["patcher"]["boxes"]

    # Iterate over tabs
    
    for box in outer_boxes:
        internal_boxes = box["box"]["patcher"]["boxes"]
        for child_box in internal_boxes:
            replace_jsarguments(child_box["box"], obj_name)

    write_json(file_edit, d)
