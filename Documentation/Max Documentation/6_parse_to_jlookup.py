import os
import json
import xml.etree.ElementTree as et
from FrameLibDocs.utils import write_json, cd_up, thin_list, strip_space
from FrameLibDocs.variables import (
    object_relationships_path,
    refpages_dir,
    package_root,
    bad_entries,
    interfaces_dir,
)
from FrameLibDocs.classes import jParseAndBuild


def main():
    """
    Creates a dict for the Max Documentation system.
    This dict contains more detailed information displayed in real-time when hovering over a certain tutorial in the umenu.

    Args:
        arg1: passes the root of the python files from the master script. Creates relative directories.
    """

    obj_lookup = os.path.join(interfaces_dir, "FrameLib-obj-jlookup.json")

    worker = jParseAndBuild()  # make an instance of the class

    # Make a list of file names and remove bad entries
    refpages = thin_list(os.listdir(refpages_dir), bad_entries)

    # Check if any files were found and do your thing
    if refpages:
        for filename in refpages:
            current_category = filename
            source_file_name = os.path.join(refpages_dir, filename)

            for filename in os.listdir(source_file_name):
                if filename != ".DS_Store":
                    source_file = os.path.join(refpages_dir, current_category, filename)
                    worker.extract_from_refpage(source_file)

        write_json(obj_lookup, worker.j_master_dict)
