from distutils.dir_util import copy_tree
from FrameLibDocs.utils import read_json, write_json
from FrameLibDocs.variables import help_dir, package_root, current_version


def append_tabs(patch, object_name, source):
    """Take a string, replace FLOBJECTNAME and append the tabs to the source"""
    modified_patch = patch.replace("FLOBJECTNAME", object_name)
    converted = json.loads(modified_patch)
    for tab in converted["patcher"]["boxes"]:
        source["patcher"]["boxes"].append(tab)


def main():

    mismatch = open(help_dir / "mismatch_template.maxhelp", "r").read()
    trigger_ins = open(help_dir / "trigger_ins_template.maxhelp", "r").read()
    templates_dir = help_dir / "templates"
    templates = [x for x in templates_dir.rglob("fl.*.maxhelp")]
    max_objects_dir = package_root / "FrameLib_Max_Objects"
    max_objects = [x for x in max_objects_dir.rglob("fl.*.cpp")]

    binary = [x for x in max_objects if x.parent.stem == "Binary"]
    ternary = [x for x in max_objects if x.parent.stem == "Ternary"]

    # Now insert the necessary tabs
    for path in templates:
        template = read_json(path)
        if path.stem in ternary:
            append_tabs(mismatch, path.stem, template)

        if path.stem in binary:
            append_tabs(trigger_ins, path.stem, template)

        write_json(path, template)

    # Now collect up and move all the templates to the dist
    # We could do this in the previous loop, but I think is clearer
    dest = current_version / "FrameLib" / "help"
    copy_tree(str(templates_dir), str(dest), update=1)
