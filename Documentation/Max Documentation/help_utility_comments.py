import argparse
import os
from pathlib import Path
from framelib.classes import Documentation
from framelib.help import comment_format


def main():
    docs = Documentation()
    parser = argparse.ArgumentParser(description="Set a folder of patchers to given formatting for live.tab")
    parser.add_argument("-path", type=dir_path)
    args = parser.parse_args()

    dir = Path(args.path)

    files = [x for x in dir.rglob("*.maxhelp")] + [x for x in dir.rglob("*.maxpat")]

    # Now fix sizes and naming

    for file in files:
        comment_format(file)

def dir_path(path):
    if os.path.isdir(path):
        return path
    else:
        raise argparse.ArgumentTypeError(f"readable_dir:{path} is not a valid path")

if __name__ == "__main__":
    main()
