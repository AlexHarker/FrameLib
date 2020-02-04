import json
import os
import xml.etree.ElementTree as et
from FrameLibDocs.utils import write_json, cd_up, check_make, strip_space
from FrameLibDocs.variables import (
    object_relationships_path,
    package_root,
    interfaces_dir,
)
from FrameLibDocs.classes import tParseAndBuild


def main():
    """
    Creates tutorial information and stores it in a dictionary format. 
    This information is displayed to the user in a umenu.
    """

    # Directory stuff #
    tutorial_index = os.path.join(
        package_root,
        "Current Test Version",
        "FrameLib",
        "docs",
        "tutorials",
        "FrameLib-tut",
        "00_fl_index.maxtut.xml",
    )
    check_make(interfaces_dir)
    obj_lookup = os.path.join(interfaces_dir, "FrameLib-obj-tlookup.json")

    worker = tParseAndBuild()

    worker.extract_from_refpage(tutorial_index)

    write_json(obj_lookup, worker.d_skeleton)
