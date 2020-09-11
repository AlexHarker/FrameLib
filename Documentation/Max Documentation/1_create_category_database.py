from FrameLibDocs.variables import package_root, category_database_path
from FrameLibDocs.utils import write_json


def main():
    """
    Creates a category database in .json format.
    Used by edit_raw_XML.py to assign object categories to the xml files.
    """

    max_objects = package_root / "FrameLib_Max_Objects"
    d = {}

    for f in max_objects.rglob("fl.*.cpp"):
        category = f"FrameLib {str(f.parent.parts[-1])}"
        if category in d:
            d[category].append(f.stem)
        else:
            d[category] = [f.stem]
    write_json(category_database_path, d)


if __name__ == "__main__":
    main()
