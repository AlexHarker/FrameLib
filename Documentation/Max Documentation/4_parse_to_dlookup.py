from FrameLibDocs.utils import write_json
from FrameLibDocs.variables import (
    refpages_dir,
    interfaces_dir,
)
from FrameLibDocs.classes import dParseAndBuild


def main():
    """
    A simplified version of the qlookup used to display information about specific objects when hovered over in the umenu.
    """
    interfaces_dir.mkdir(exist_ok=True)
    obj_lookup = interfaces_dir / "FrameLib-obj-dlookup.json"

    worker = dParseAndBuild()

    refpages = [x for x in refpages_dir.rglob("fl.*.xml")]

    for ref in refpages:
        worker.extract_from_refpage(ref)

    write_json(obj_lookup, worker.d_master_dict)


if __name__ == "__main__":
    main()
