import xml.etree.ElementTree as et
import re
import json
from shutil import copyfile
from framelib.utils import write_json, read_json
from framelib.classes import Documentation


def is_details(box: dict) -> bool:
    return "jsarguments" in box and box["filename"] == "fl.helpdetails.js"

def is_jshelp(box: dict) -> bool:
    return "jsarguments" in box and (is_details(box) or box["filename"] == "fl.helpname.js")

def append_tabs(patch, source, find_object, replace_object):
    """Take a string of the raw patch file, append the tabs to the source"""
    modified_patch = patch.replace(find_object, replace_object)
    converted = json.loads(modified_patch)
    for tab in converted["patcher"]["boxes"]:
        source["patcher"]["boxes"].append(tab)

def live_tab_format_patch(patch: dict, size: float, font: str) -> bool:
    boxes = patch["patcher"]["boxes"]
    local_found = False
    for item in boxes:
        box = item["box"]
        if box["maxclass"] == "live.tab":
            box["fontsize"] = size
            box["fontname"] = font
            local_found = True
        if "patcher" in box:
            recurse = live_tab_format_patch(box, size, font)
            local_found = local_found or recurse
    return local_found

def live_tab_format(path: str, size: float, font: str):
    """Apply formatting to all live.tabs"""
    d = read_json(path)

    found = live_tab_format_patch(d, size, font)

    if found:
        write_json(path, d)

def comment_format_patch(patch: dict, depth: int) -> bool:
    boxes = patch["patcher"]["boxes"]
    local_found = False
    for item in boxes:
        box = item["box"]
        if box["maxclass"] == "comment" and ("bubble" not in box or box["bubble"] == 0) and ("fontname" not in box or box["fontname"] == "Arial"):
            box["textcolor"] = [0.5, 0.5, 0.5, 1.0]
            local_found = True
        if "patcher" in box:
            if depth < 1:
                recurse = comment_format_patch(box, depth + 1)
                local_found = local_found or recurse
    return local_found

def comment_format(path: str):
    """Apply formatting to all comments"""
    d = read_json(path)

    found = comment_format_patch(d, 0)

    if found:
        write_json(path, d)

def highlight_reusable_tabs(patch, source, object, find_object):
    """First find the highlight colour and then apply it to the input tab"""
    json_patch = json.loads(patch)

    boxes = source["patcher"]["boxes"]
    for item in boxes:
        if item["box"]["text"] == "p basic":
            tab_boxes = item["box"]["patcher"]["boxes"]
            for child in tab_boxes:
                box = child["box"]
                if box["maxclass"] == "newobj" and box["text"].startswith(object):
                    colour = box["color"]

    boxes = json_patch["patcher"]["boxes"]
    for item in boxes:
        tab_boxes = item["box"]["patcher"]["boxes"]
        for child in tab_boxes:
            box = child["box"]
            if box["maxclass"] == "newobj" and box["text"].startswith(find_object):
                box["color"] = colour
            if box["maxclass"] == "multislider":
                box["slidercolor"] = colour

    return json.JSONEncoder().encode(json_patch)

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

    description = "".join(root.find("description").itertext()).strip()
    description = re.sub(r"More info.+Tutorial.+\.", " ", description);
    pattern = r".{1," + str(width) + r"}(\s|$)|\S+?(\s|$)";

    matches = re.finditer(pattern, description);
    lines = 0;

    for m in matches:
        lines = lines + 1

    if lines > 5:
        print("long details warning: " + name + " lines " + str(lines))

    return lines

def resize_patch(patch: dict, width: float, height: float, pad: float, num_characters: int, lines: int, js_resize: bool) -> int:
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
                box["patching_rect"][3] = 80 + lines * 15
                box["jsarguments"] = [ "objname", num_characters ]
                return lines

    return 0

def resize_help(docs: Documentation, path: str, width: float, height: float, detail_factor: float = 5.8) -> None:
    """Takes a path to a Max patch and resizes the window for the main patch and all tabs"""
    d = read_json(path)

    pad = 15
    height_reduce = 26
    num_characters = int((width - (pad * 2)) / detail_factor)
    lines = detail_lines(docs, path.stem, num_characters)
    details_offset = pad + 97 + lines * 15

    outer_patch = d["patcher"]
    resize_patch(outer_patch, width, height, pad, num_characters, lines, False)
    outer_boxes = outer_patch["boxes"]

    # Iterate over tabs

    for item in outer_boxes:
        inner_patch = item["box"]["patcher"]
        resize_patch(inner_patch, width, height - height_reduce, pad, num_characters, lines, True)

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
