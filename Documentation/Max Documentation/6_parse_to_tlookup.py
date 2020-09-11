import json
import os
import xml.etree.ElementTree as et
from FrameLibDocs.utils import write_json, strip_space
from FrameLibDocs.variables import (
    object_relationships_path,
    current_version,
    interfaces_dir,
)
from FrameLibDocs.classes import tParseAndBuild


def main():
    """
    Creates tutorial information and stores it in a dictionary format. 
    This information is displayed to the user in a umenu.
    """

    tutorial_index = current_version / "FrameLib" / "docs" / "tutorials" / "FrameLib-tut" / "00_fl_index.maxtut.xml"
    interfaces_dir.mkdir(exist_ok=True)

    obj_lookup = interfaces_dir / "FrameLib-obj-tlookup.json"

    worker = tParseAndBuild()

    worker.extract_from_refpage(tutorial_index)

    write_json(obj_lookup, worker.d_skeleton)

if __name__ == "__main__":
    main()
