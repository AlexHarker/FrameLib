import xml.etree.ElementTree as et
from framelib.utils import write_json, strip_space
from framelib.classes import jParseAndBuild, Documentation


def main(docs: Documentation):
    """
    Creates a dict for the Max Documentation system.
    This dict contains more detailed information displayed in real-time when hovering over a certain tutorial in the umenu.
    """

    obj_lookup = docs.interfaces_dir / "framelib-obj-jlookup.json"

    worker = jParseAndBuild()

    refpages = [x for x in docs.refpages_dir.rglob("fl.*.xml")]

    for ref in refpages:
        worker.extract_from_refpage(ref)

    write_json(obj_lookup, worker.j_master_dict)


if __name__ == "__main__":
    main(Documentation())
