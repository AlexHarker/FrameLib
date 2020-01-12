import os
import json
from FrameLibDocs.utils import cd_up, write_json, thin_list, check_make, strip_space
from FrameLibDocs.variables import (
    package_root,
    refpages_dir,
    bad_entries,
    interfaces_dir,
)
from FrameLibDocs.classes import dParseAndBuild


def main():
    """
    A simplified version of the qlookup used to display information about specific objects when hovered over in the umenu.
    The qlookup might replace this entire script.

    Args:
        arg1: passes the root of the python files from the master script. Creates relative directories.
    """
    check_make(interfaces_dir)
    obj_lookup = os.path.join(interfaces_dir, "FrameLib-obj-dlookup.json")

    worker = dParseAndBuild()

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

        write_json(obj_lookup, worker.d_master_dict)
    else:
        print("Found no XML files to parse. Moving on to next stage.")
