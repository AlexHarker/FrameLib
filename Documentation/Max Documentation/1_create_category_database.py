from FrameLibDocs.variables import package_root, category_database_path
from FrameLibDocs.utils import write_json


def main():
    """
    This creates a category database in .json format.
    This is used by edit_raw_XML.py to assign object categories to the xml files.
    """

    max_objects = package_root / "FrameLib_Max_Objects"
    d = {}

    for f in max_objects.rglob("fl.*.cpp"):
        category = f"FrameLib {f.parent.parts[-1]}"
        if category in d:
            d[category].append(f.name)
        else:
            d[category] = [f.name]
    write_json(category_database_path, d)

if __name__ == "__main__":
    main()
