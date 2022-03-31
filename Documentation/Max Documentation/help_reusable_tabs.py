import json
from distutils.dir_util import copy_tree
from framelib.utils import read_json, write_json
from framelib.classes import Documentation
from framelib.help import edit_help


def append_tabs(patch, source, find_object, replace_object):
    """Take a string, append the tabs to the source"""
    modified_patch = patch.replace(find_object, replace_object)
    converted = json.loads(modified_patch)
    for tab in converted["patcher"]["boxes"]:
        source["patcher"]["boxes"].append(tab)

def lookup_input_string(object) -> str:

    dict = {    "fl.ramp~": "/scale normalised",
                "fl.uniform~": "/value 0.657",
                "fl.gaussian~": "/deviation 0.1",
                "fl.makewindow~": "/window kaiser",
                "fl.random~": ""     }
                
    return object + " " + dict[object]

def fix_mapping(patch, name):
    if name == "fl.and~" or name == "fl.or~":
        return patch.replace("linear 0.2 0.8 0.2 0.8 1", "linear 0.5 0.8 0. 0.8 1")
    if name == "fl.copysign~":
        return patch.replace("linear 0.2 0.8 0.2 0.8 1", "linear 0.2 0.8 -0.8 0.8 1", 1)
    return patch

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
    
        name = docs.refpage_name(path.stem)
        
        if path.stem in ternary:
            append_tabs(mismatch_ternary, template, "fl.fold~", name)

        if path.stem in binary:
            trigger_ins_fixed = fix_mapping(trigger_ins, path.stem)
            append_tabs(trigger_ins_fixed, template, "fl.*~", name)
            append_tabs(mismatch_binary, template, "fl.-~", name)
        
        if path.stem in complex_binary:
            append_tabs(trigger_ins_complex, template, "fl.complex.*~", name)
            append_tabs(mismatch_complex, template, "fl.complex.-~", name)
        
        if path.stem in generators:
            append_tabs(in_mode, template, "fl.random~", lookup_input_string(name))

        write_json(path, template)
        
        # Now ensure the arguments to js help file objects are correct
        
        edit_help(docs, path, path.stem)

    # Now collect up and move all the templates to the dist
    # We could do this in the previous loop, but I think is clearer
    dest = docs.package / "help"
    copy_tree(str(docs.help_templates_dir), str(dest), update=1)

if __name__ == "__main__":
    main(Documentation())
