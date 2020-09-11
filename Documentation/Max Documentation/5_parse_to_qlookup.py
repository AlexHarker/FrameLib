from FrameLibDocs.utils import write_json, read_yaml
from FrameLibDocs.variables import (
    object_relationships_path,
    refpages_dir,
    interfaces_dir,
)
from FrameLibDocs.classes import qParseAndBuild


def main():
    """
    Creates a dict for the Max Documentation system.
    This dict contains is essential for maxObjectLauncher/Refpages to pull the right info.
    """

    object_info = read_yaml(object_relationships_path)
    interfaces_dir.mkdir(exist_ok=True)
    obj_lookup = interfaces_dir / "FrameLib-obj-qlookup.json"

    worker = qParseAndBuild()

    refpages = [x for x in refpages_dir.rglob("fl.*.xml")]

    for ref in refpages:
        worker.extract_from_refpage(ref)
        worker.extract_keywords(object_info)
        worker.extract_seealso(object_info)
        worker.build_json_file()

    write_json(obj_lookup, worker.d_master_dict)

if __name__ == "__main__":
    main()
