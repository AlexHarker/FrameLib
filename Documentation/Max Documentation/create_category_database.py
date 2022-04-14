# from FrameLibDocs.variables import repo_root, category_database_path
from framelib.utils import write_json
from framelib.classes import Documentation


def main(docs):
    """
    Creates a category database in .json format.
    Used by edit_raw_XML.py to assign object categories to the xml files.
    """

    d = {}

    for f in docs.source_files:
        category = f"FrameLib {str(f.parent.parts[-1])}"
        if category in d:
            d[category].append(f.stem)
        else:
            d[category] = [f.stem]
    write_json(docs.category_database_path, d)

if __name__ == "__main__":
    main(Documentation())
