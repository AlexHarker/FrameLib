import json
from distutils.dir_util import copy_tree
from framelib.utils import read_json, write_json
from framelib.classes import Documentation
from framelib.help import rename_help, auto_resize_help


def in_group(docs, path, type):
    return path.stem in [x.stem for x in docs.source_files if x.parent.stem == type]

def in_custom_list(path, object_list):
    return path.stem in object_list

def main(docs: Documentation):

    templates = [x for x in docs.help_templates_dir.rglob("fl.*.maxhelp")]

    object_list1 = [ "fl.chain~", "fl.combinetags~", "fl.read~", "fl.sink~", "fl.ticks~", "fl.framemin~", "fl.framemax~", "fl.kurtosis~", "fl.nonzero~" ]
    object_list2 = [ "fl.movingaverage~", "fl.convolve~", "fl.fft~", "fl.gaussian~", "fl.makewindow~", "fl.random~", "fl.recall~" ]

    # Now fix sizes and naming

    for path in templates:

        name = docs.refpage_name(path.stem)
        details_factor = 5.75;

        # Special cases for details_factor

        if in_group(docs, path, "Binary"):
            details_factor = 5.7
        elif in_custom_list(path, object_list1):
            details_factor = 5.8
        elif in_custom_list(path, object_list2):
            details_factor = 5.9

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
