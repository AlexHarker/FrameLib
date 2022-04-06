import json
from distutils.dir_util import copy_tree
from framelib.utils import read_json, write_json
from framelib.classes import Documentation
from framelib.help import rename_help, auto_resize_help


def main(docs: Documentation):

    templates = [x for x in docs.help_templates_dir.rglob("fl.*.maxhelp")]

    binary = [x.stem for x in docs.source_files if x.parent.stem == "Binary"]
    ternary = [x.stem for x in docs.source_files if x.parent.stem == "Ternary"]
    complex_binary = [x.stem for x in docs.source_files if x.parent.stem == "Complex_Binary"]
    time_smoothing = [x.stem for x in docs.source_files if x.parent.stem == "Time_Smoothing"]
    
    # Now fix sizes and naming

    for path in templates:

        name = docs.refpage_name(path.stem)
        details_factor = 5.8;

        # Special cases for details_factor

        if path.stem in ternary:
            details_factor = 5.55
        elif path.stem in binary:
            details_factor = 5.7
        elif path.stem in complex_binary:
            details_factor = 5.75
        elif path.stem in time_smoothing:
            details_factor = 5.76

        # Resize

        auto_resize_help(docs, path, details_factor)

        # Now ensure the arguments to js help file objects are correct

        rename_help(docs, path, path.stem)

    # Now collect up and move all the templates to the dist
    # We could do this in the previous loop, but I think is clearer
    dest = docs.package / "help"
    copy_tree(str(docs.help_templates_dir), str(dest), update=1)

if __name__ == "__main__":
    main(Documentation())
