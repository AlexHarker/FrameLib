import os
from shutil import rmtree
from framelib.classes import Documentation

def main(docs: Documentation, clean: bool=True):
    docs = Documentation()
    docs.temporary_dir.mkdir(exist_ok=True)
    docs.databases_dir.mkdir(exist_ok=True)
    docs.raw_xml_dir.mkdir(exist_ok=True)
    docs.help_templates_dir.mkdir(exist_ok=True)

    if clean:
        # Interfaces
        for file in docs.interfaces_dir.iterdir():
            file.unlink()

        for file in docs.temporary_dir.iterdir():
            if not file.is_dir():
                file.unlink()

            # Refpages
            for file in docs.refpages_dir.iterdir():
                if file.is_dir():
                    try:
                        rmtree(file.resolve())
                    except OSError:
                        print("Error cleaning out existing docs directories")

if __name__ == "__main__":
    main(Documentation())
