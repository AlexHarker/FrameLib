from shutil import copyfile
from framelib.utils import write_json, read_json
from framelib.aliases import refpage_name


def edit_help(docs: str, file_edit: str, obj_name: str) -> None:
    """Takes a path to a Max patch and does a find and replace on object names"""
    obj_name = refpage_name(docs, obj_name)
    d = read_json(file_edit)
    outer_boxes = d["patcher"]["boxes"][0]
    internal_boxes = outer_boxes["box"]["patcher"]["boxes"]

    for key in internal_boxes:
        if "jsarguments" in key["box"]:
            key["box"]["jsarguments"] = obj_name

    write_json(file_edit, d)
