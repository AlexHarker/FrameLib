from shutil import copyfile
from framelib.utils import write_json, read_json
from framelib.classes import Documentation

def is_details(box: dict) -> bool:
    return "jsarguments" in box and box["filename"] == "fl.helpdetails.js"

def is_jshelp(box: dict) -> bool:
    return "jsarguments" in box and (is_details(box) or box["filename"] == "fl.helpname.js")

def rename_help(docs: Documentation, file_edit: str, obj_name: str) -> None:
    """Takes a path to a Max patch and does a find and replace on object names"""
    obj_name = docs.refpage_name(obj_name)
    d = read_json(file_edit)

    outer_boxes = d["patcher"]["boxes"]

    # Iterate over tabs

    for item in outer_boxes:
        inner_boxes = item["box"]["patcher"]["boxes"]
        for child in inner_boxes:
            box = child["box"]
            if is_jshelp(box):
                if isinstance(box["jsarguments"], list) and len(box["jsarguments"]) > 1:
                    box["jsarguments"][0] = obj_name
                else:
                    box["jsarguments"] = obj_name

    write_json(file_edit, d)

def resize_patch(patch: dict, width: float, height: float, pad: float, js_resize: bool) -> None:
    """Takes a patcher as a dict and resizes it"""
    patch["rect"][2] = width
    patch["rect"][3] = height

    if js_resize:
        boxes = patch["boxes"]
        for item in boxes:
            box = item["box"]
            if is_jshelp(box):
                box["patching_rect"][2] = width - (pad * 2)
            if is_details(box):
                box["jsarguments"] = [ "objname", int(width / 6.3) ]

def resize_help(docs: Documentation, file_edit: str, width: float, height: float) -> None:
    """Takes a path to a Max patch and resizes the window for the main patch and all tabs"""
    d = read_json(file_edit)

    details_offset = 167
    pad = 15
    height_reduce = 26

    outer_patch = d["patcher"]
    resize_patch(outer_patch, width, height, pad, False)
    outer_boxes = outer_patch["boxes"]

    # Iterate over tabs

    for item in outer_boxes:
        inner_patch = item["box"]["patcher"]
        resize_patch(inner_patch, width, height - height_reduce, pad, True)

    # Resize parameter detail bpatchers

    for item in outer_boxes:
        if item["box"]["text"] == "p info":
            tab_boxes = item["box"]["patcher"]["boxes"]
            for child in tab_boxes:
                box = child["box"]
                if box["maxclass"] == "bpatcher" and box["name"] == "fl.docs.getparams.maxpat":
                    box["patching_rect"][2] = width - (pad * 2)
                    box["patching_rect"][3] = height - (details_offset + height_reduce + pad)
                    box["args"] = [ width - (pad * 2) ]

    write_json(file_edit, d)

def auto_resize_help(docs: Documentation, file_edit: str) -> None:
    """Takes a path to a Max patch and automatically resizes the window for the main patch and all tabs"""
    d = read_json(file_edit)

    height_reduce = 26

    outer_patch = d["patcher"]
    outer_boxes = outer_patch["boxes"]

    # Minimum width is the size of the umenu in fl.docs.getparams (352) + 4 per side

    width = 352
    height = 0

    # Iterate over tabs

    for item in outer_boxes:
        if item["box"]["text"] != "p info" and item["box"]["text"] != "p ?":
            inner_patch = item["box"]["patcher"]
            rect = inner_patch["rect"]
            width = max(width, rect[2])
            height = max(height, rect[3])

    resize_help(docs, file_edit, width, height + height_reduce )
