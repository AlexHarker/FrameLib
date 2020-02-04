import os
import yaml
import xml.etree.ElementTree as et
from FrameLibDocs.utils import (
    write_json,
    cd_up,
    thin_list,
    read_yaml,
    check_make,
    strip_space,
)
from FrameLibDocs.variables import (
    object_relationships_path,
    refpages_dir,
    package_root,
    bad_entries,
    interfaces_dir,
)
from FrameLibDocs.classes import qParseAndBuild


def main():
    """
    Creates a dict for the Max Documentation system.
    This dict contains is essential for maxObjectLauncher/Refpages to pull the right info.
    """

    object_info = read_yaml(object_relationships_path)
    check_make(interfaces_dir)
    obj_lookup = os.path.join(interfaces_dir, "FrameLib-obj-qlookup.json")

    worker = qParseAndBuild()

    # Make a list of file names and remove bad entries
    refpages = thin_list(os.listdir(refpages_dir), bad_entries)

    # Check if any files were found and do your thing
    if refpages:
        for filename in refpages:
            current_category = filename
            source_file_name = os.path.join(refpages_dir, filename)

            for filename in os.listdir(source_file_name):
                source_file = os.path.join(refpages_dir, current_category, filename)
                worker.extract_from_refpage(source_file)
                worker.extract_keywords(object_info)
                worker.extract_seealso(object_info)
                worker.build_json_file()

        # Write out to JSON
        write_json(obj_lookup, worker.d_master_dict)
