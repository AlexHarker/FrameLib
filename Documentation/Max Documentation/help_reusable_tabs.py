import json
from distutils.dir_util import copy_tree
from framelib.utils import read_json, write_json
from framelib.classes import Documentation
from framelib.help import edit_help


def append_tabs(docs, patch, source, find_object, replace_object):
    """Take a string, append the tabs to the source"""
    modified_patch = patch.replace(find_object, docs.refpage_name(replace_object))
    converted = json.loads(modified_patch)
    for tab in converted["patcher"]["boxes"]:
        source["patcher"]["boxes"].append(tab)


def main(docs: Documentation):
    mismatch_binary = open(docs.help_dir / "reusable_tabs" / "mismatch_binary_template.maxhelp", "r").read()
    mismatch_ternary = open(docs.help_dir / "reusable_tabs" / "mismatch_ternary_template.maxhelp", "r").read()
    mismatch_complex = open(docs.help_dir / "reusable_tabs" / "mismatch_complex_template.maxhelp", "r").read()
    trigger_ins = open(docs.help_dir / "reusable_tabs" / "trigger_ins_template.maxhelp", "r").read()
    trigger_ins_complex = open(docs.help_dir / "reusable_tabs" / "trigger_ins_complex_template.maxhelp", "r").read()
    in_mode = open(docs.help_dir / "reusable_tabs" / "input_mode_template.maxhelp").read()

    templates = [x for x in docs.help_templates_dir.rglob("fl.*.maxhelp")]

    binary = [x.stem for x in docs.source_files if x.parent.stem == "Binary"]
    ternary = [x.stem for x in docs.source_files if x.parent.stem == "Ternary"]
    complex_binary = [x.stem for x in docs.source_files if x.parent.stem == "Complex_Binary"]
    generators = [x.stem for x in docs.source_files if x.parent.stem == "Generators"]

    # Now insert the necessary tabs
    for path in templates:
        template = read_json(path)
    
        if path.stem in ternary:
            append_tabs(docs, mismatch_ternary, template, "fl.fold~", path.stem)

        if path.stem in binary:
            append_tabs(docs, trigger_ins, template, "objectnameneedsentering", path.stem)
            append_tabs(docs, mismatch_binary, template, "fl.-~", path.stem)
        
        if path.stem in complex_binary:
            append_tabs(docs, trigger_ins_complex, template, "objectnameneedsentering", path.stem)
            append_tabs(docs, mismatch_complex, template, "fl.complex.-~", path.stem)
        
        if path.stem in generators:
            append_tabs(docs, in_mode, template, "objectnameneedsentering", path.stem)

        write_json(path, template)
        
        # Now ensure the arguments to js help file objects are correct
        
        edit_help(docs, path, path.stem)

    # Now collect up and move all the templates to the dist
    # We could do this in the previous loop, but I think is clearer
    dest = docs.package / "help"
    copy_tree(str(docs.help_templates_dir), str(dest), update=1)

if __name__ == "__main__":
    main(Documentation())
