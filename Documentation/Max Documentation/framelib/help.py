from shutil import copyfile
from FrameLibDocs.utils import write_json, read_json


def edit_help(file_edit: str, obj_name: str) -> None:
    """Takes a path to a Max patch and does a find and replace on object names"""
    d = read_json(file_edit)
    outer_boxes = d["patcher"]["boxes"][0]
    internal_boxes = outer_boxes["box"]["patcher"]["boxes"]

    for key in internal_boxes:
        if "jsarguments" in key["box"]:
            key["box"]["jsarguments"] = obj_name

    write_json(file_edit, d)
