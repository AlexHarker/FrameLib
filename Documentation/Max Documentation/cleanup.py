import shutil
from framelib.classes import Documentation


def main(docs: Documentation):
    shutil.rmtree(docs.temporary_dir)

if __name__ == "__main__":
    main(Documentation())
