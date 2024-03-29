import json, os
import xml.etree.ElementTree as et
from framelib.utils import write_json, strip_space
from framelib.classes import tParseAndBuild, Documentation


def main(docs: Documentation):
    """
    Creates tutorial information and stores it in a dictionary format.
    This information is displayed to the user in a umenu.
    """

    tutorial_index = (
        docs.package / "docs" / "tutorials" / "framelib-tut" / "00_fl_index.maxtut.xml"
    )
    docs.interfaces_dir.mkdir(exist_ok=True)

    obj_lookup = docs.interfaces_dir / "framelib-obj-tlookup.json"

    worker = tParseAndBuild()

    worker.extract_from_refpage(tutorial_index)

    write_json(obj_lookup, worker.d_skeleton)


if __name__ == "__main__":
    main(Documentation())
