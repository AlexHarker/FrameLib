import os
from shutil import rmtree
from FrameLibDocs.variables import (
    max_docs_dir,
    temporary_dir,
    databases_dir,
    raw_xml_dir,
    interfaces_dir,
    refpages_dir,
)

from FrameLibDocs.classes import Documentation

# Check and make

def main(docs: Documentation):
    docs.temporary_dir.mkdir(exist_ok=True)
    docs.databases_dir.mkdir(exist_ok=True)
    docs.raw_xml_dir.mkdir(exist_ok=True)

    # Interfaces
    for files in interfaces_dir.iterdir():
        files.unlink()

    # Refpages
    for files in refpages_dir.iterdir():
        if files.is_dir():
            try:
                rmtree(files.resolve())
            except OSError:
                print("Error cleaning out existing docs directories")

if __name__ == "__main__":
    main(Documentation())
