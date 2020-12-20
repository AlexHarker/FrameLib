import os, json
import xml.etree.ElementTree as et
from FrameLibDocs.utils import write_json, strip_space
from FrameLibDocs.variables import (
    interfaces_dir,
    refpages_dir,
)
from FrameLibDocs.classes import jParseAndBuild


def main():
    """
    Creates a dict for the Max Documentation system.
    This dict contains more detailed information displayed in real-time when hovering over a certain tutorial in the umenu.
    """

    obj_lookup = interfaces_dir / "FrameLib-obj-jlookup.json"

    worker = jParseAndBuild()

    refpages = [x for x in refpages_dir.rglob("fl.*.xml")]

    for ref in refpages:
        worker.extract_from_refpage(ref)

    write_json(obj_lookup, worker.j_master_dict)


if __name__ == "__main__":
    main()
