import json
from distutils.dir_util import copy_tree
from framelib.utils import read_json, write_json
from framelib.classes import Documentation


def append_tabs(patch, object_name, source):
    """Take a string, replace FLOBJECTNAME and append the tabs to the source"""
    modified_patch = patch.replace("FLOBJECTNAME", object_name)
    converted = json.loads(modified_patch)
    for tab in converted["patcher"]["boxes"]:
        source["patcher"]["boxes"].append(tab)


def main(docs: Documentation):
    mismatch = open(docs.help_dir / "mismatch_template.maxhelp", "r").read()
    trigger_ins = open(docs.help_dir / "trigger_ins_template.maxhelp", "r").read()
    in_mode = open(docs.help_dir / "input_mode_template.maxhelp").read()
    templates_dir = docs.help_dir / "templates"
    templates = [x for x in templates_dir.rglob("fl.*.maxhelp")]

    binary = [x.stem for x in docs.source_files if x.parent.stem == "Binary"]
    ternary = [x.stem for x in docs.source_files if x.parent.stem == "Ternary"]
    complex_binary = [x.stem for x in docs.source_files if x.parent.stem == "Complex_Binary"]
    generators = [x.stem for x in docs.source_files if x.parent.stem == "Generators"]

    # Now insert the necessary tabs
    for path in templates:
        template = read_json(path)
        if path.stem in ternary:
            append_tabs(mismatch, path.stem, template)

        if path.stem in binary:
            append_tabs(trigger_ins, path.stem, template)
            append_tabs(mismatch, path.stem, template)
        
        if path.stem in complex_binary:
            append_tabs(trigger_ins, path.stem, template)
            append_tabs(mismatch, path.stem, template)
        
        if path.stem in generators:
            append_tabs(in_mode, path.stem, template)

        write_json(path, template)

    # Now collect up and move all the templates to the dist
    # We could do this in the previous loop, but I think is clearer
    dest = docs.package / "FrameLib" / "help"
    copy_tree(str(templates_dir), str(dest), update=1)

if __name__ == "__main__":
    main(Documentation())
