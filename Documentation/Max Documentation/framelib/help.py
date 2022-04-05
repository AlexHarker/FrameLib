import xml.etree.ElementTree as et
import re
from shutil import copyfile
from framelib.utils import write_json, read_json
from framelib.classes import Documentation


def is_details(box: dict) -> bool:
    return "jsarguments" in box and box["filename"] == "fl.helpdetails.js"

def is_jshelp(box: dict) -> bool:
    return "jsarguments" in box and (is_details(box) or box["filename"] == "fl.helpname.js")

def rename_help(docs: Documentation, path: str, obj_name: str) -> None:
    """Takes a path to a Max patch and does a find and replace on object names"""
    obj_name = docs.refpage_name(obj_name)
    d = read_json(path)

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

    write_json(path, d)

def detail_lines(docs: Documentation, name: str, width: int) -> int:

    tree = et.parse(docs.refpages_dir / "framelib-ref" / (name + ".maxref.xml"))  # parse the xml file
    root = tree.getroot()

    description = root.find("description").text.strip()

    pattern = r".{1," + str(width) + r"}(\s|$)|\S+?(\s|$)";

    matches = re.finditer(pattern, description);
    lines = 0;

    for m in matches:
        lines = lines + 1

    if lines > 4:
        print("details issue: " + name + " lines " + str(lines))

    return lines

def resize_patch(docs: Documentation, obj_name: str, patch: dict, width: float, height: float, pad: float, detail_factor: float, js_resize: bool) -> int:
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
                num_characters = int((width - (pad * 2)) / detail_factor)
                lines = detail_lines(docs, obj_name, num_characters)
                box["patching_rect"][3] = 80 + lines * 15
                box["jsarguments"] = [ "objname", num_characters ]
                return lines

    return 0

def resize_help(docs: Documentation, path: str, width: float, height: float, detail_factor: float = 5.8) -> None:
    """Takes a path to a Max patch and resizes the window for the main patch and all tabs"""
    d = read_json(path)

    details_offset = 167
    pad = 15
    height_reduce = 26

    outer_patch = d["patcher"]
    resize_patch(docs, path.stem, outer_patch, width, height, pad, detail_factor, False)
    outer_boxes = outer_patch["boxes"]

    # Iterate over tabs

    for item in outer_boxes:
        inner_patch = item["box"]["patcher"]
        lines = resize_patch(docs, path.stem, inner_patch, width, height - height_reduce, pad, detail_factor, True)
        if lines:
            details_offset = pad + 97 + lines * 15

    # Resize parameter detail bpatchers

    for item in outer_boxes:
        if item["box"]["text"] == "p info":
            tab_boxes = item["box"]["patcher"]["boxes"]
            for child in tab_boxes:
                box = child["box"]
                if box["maxclass"] == "bpatcher" and box["name"] == "fl.docs.getparams.maxpat":
                    box["patching_rect"][1] = details_offset
                    box["patching_rect"][2] = width - (pad * 2)
                    box["patching_rect"][3] = height - (details_offset + height_reduce + pad)
                    box["args"] = [ width - (pad * 2) ]

    write_json(path, d)

def auto_resize_help(docs: Documentation, path: str, detail_factor:float = 5.8) -> None:
    """Takes a path to a Max patch and automatically resizes the window for the main patch and all tabs"""
    d = read_json(path)

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

    resize_help(docs, path, width, height + height_reduce, detail_factor)
