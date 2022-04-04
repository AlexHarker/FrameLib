import json
from distutils.dir_util import copy_tree
from framelib.utils import read_json, write_json
from framelib.classes import Documentation
from framelib.help import rename_help, auto_resize_help


def main(docs: Documentation):

    templates = [x for x in docs.help_templates_dir.rglob("fl.*.maxhelp")]

    unary = [x.stem for x in docs.source_files if x.parent.stem == "Unary"]
    binary = [x.stem for x in docs.source_files if x.parent.stem == "Binary"]
    ternary = [x.stem for x in docs.source_files if x.parent.stem == "Ternary"]
    complex_unary = [x.stem for x in docs.source_files if x.parent.stem == "Complex_Unary"]
    complex_binary = [x.stem for x in docs.source_files if x.parent.stem == "Complex_Binary"]
    expressions = [x.stem for x in docs.source_files if x.parent.stem == "Expressions"]
    host_communication = [x.stem for x in docs.source_files if x.parent.stem == "Host_Communication"]
    io = [x.stem for x in docs.source_files if x.parent.stem == "IO"]
    mapping = [x.stem for x in docs.source_files if x.parent.stem == "Mapping"]
    schedulers = [x.stem for x in docs.source_files if x.parent.stem == "Schedulers"]
    spectral = [x.stem for x in docs.source_files if x.parent.stem == "Spectral"]
    streaming = [x.stem for x in docs.source_files if x.parent.stem == "Streaming"]
    timing = [x.stem for x in docs.source_files if x.parent.stem == "Timing"]
    time_smoothing = [x.stem for x in docs.source_files if x.parent.stem == "Time_Smoothing"]

    auto_resize_list = unary + complex_unary + expressions + schedulers + host_communication + io + timing + streaming + spectral + mapping
    # Now fix sizes and naming

    for path in templates:

        name = docs.refpage_name(path.stem)

        # Resize

        if path.stem in auto_resize_list:
            auto_resize_help(docs, path)

        if path.stem in ternary:
            auto_resize_help(docs, path, 5.55)

        if path.stem in binary:
            auto_resize_help(docs, path, 5.7)

        if path.stem in complex_binary:
            auto_resize_help(docs, path, 5.75)

        if path.stem in time_smoothing:
            auto_resize_help(docs, path, 5.76)

        # Now ensure the arguments to js help file objects are correct

        rename_help(docs, path, path.stem)

    # Now collect up and move all the templates to the dist
    # We could do this in the previous loop, but I think is clearer
    dest = docs.package / "help"
    copy_tree(str(docs.help_templates_dir), str(dest), update=1)

if __name__ == "__main__":
    main(Documentation())
