import argparse
import os
from pathlib import Path
from framelib.classes import Documentation
from framelib.help import resize_help


def main():
    docs = Documentation()
    parser = argparse.ArgumentParser(description="Set a folder of helpfiles to a given size")
    parser.add_argument("-path", type=dir_path)
    parser.add_argument("-width", default=200, type=int, help="Width of patches")
    parser.add_argument("-height", default=200, type=int, help="Height of patches")
    args = parser.parse_args()

    dir = Path(args.path)
    
    files = [x for x in dir.rglob("fl.*.maxhelp")]

    # Now fix sizes and naming

    for file in files:

        resize_help(docs, file, args.width, args.height)

def dir_path(path):
    if os.path.isdir(path):
        return path
    else:
        raise argparse.ArgumentTypeError(f"readable_dir:{path} is not a valid path")

if __name__ == "__main__":
    main()
