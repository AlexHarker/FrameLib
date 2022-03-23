import os
from shutil import rmtree
from framelib.classes import Documentation

def main(docs: Documentation):
    docs = Documentation()
    docs.temporary_dir.mkdir(exist_ok=True)
    docs.databases_dir.mkdir(exist_ok=True)
    docs.raw_xml_dir.mkdir(exist_ok=True)

    # Interfaces
    for files in docs.interfaces_dir.iterdir():
        files.unlink()

    # Refpages
    for files in docs.refpages_dir.iterdir():
        if files.is_dir():
            try:
                rmtree(files.resolve())
            except OSError:
                print("Error cleaning out existing docs directories")

if __name__ == "__main__":
    main(Documentation())

