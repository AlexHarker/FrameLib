import json
from framelib.utils import read_json, write_json
from framelib.classes import Documentation
from framelib.help import append_tabs, highlight_reusable_tabs


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

    unary = [x.stem for x in docs.source_files if x.parent.stem == "Unary"]
    binary = [x.stem for x in docs.source_files if x.parent.stem == "Binary"]
    ternary = [x.stem for x in docs.source_files if x.parent.stem == "Ternary"]
    complex_binary = [x.stem for x in docs.source_files if x.parent.stem == "Complex_Binary"]
    generators = [x.stem for x in docs.source_files if x.parent.stem == "Generators"]

    # Now insert the necessary tabs
    for path in templates:

        name = docs.refpage_name(path.stem)

        if path.stem in ternary:
            template = read_json(path)
            append_tabs(mismatch_ternary, template, "fl.fold~", name)
            write_json(path, template)

        if path.stem in binary:
            template = read_json(path)
            trigger_ins_fixed = fix_mapping(trigger_ins, path.stem)
            append_tabs(trigger_ins_fixed, template, "fl.*~", name)
            append_tabs(mismatch_binary, template, "fl.-~", name)
            write_json(path, template)

        if path.stem in complex_binary:
            template = read_json(path)
            append_tabs(trigger_ins_complex, template, "fl.complex.*~", name)
            append_tabs(mismatch_complex, template, "fl.complex.-~", name)
            write_json(path, template)

        if path.stem in generators:
            template = read_json(path)
            patch = highlight_reusable_tabs(in_mode, template, name, "fl.random~")
            append_tabs(patch, template, "fl.random~", lookup_input_string(name))
            write_json(path, template)

if __name__ == "__main__":
    main(Documentation())
