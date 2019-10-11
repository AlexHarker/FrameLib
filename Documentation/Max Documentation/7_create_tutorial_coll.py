import os
import re
from FrameLibDocs.utils import cd_up

def try_integer(s):
    try:
        return int(s)
    except ValueError:
        return s

def alphanum_key(s):
    """ 
    Turn a string into a list of string and number chunks.
    """
    return [ try_integer(c) for c in re.split('([0-9]+)', s) ]


def main(root):
    # Directory stuff
    dir_path = root
    dir_path = os.path.join(cd_up(root, 2), "Current Test Version", "FrameLib")
    tutorial_path = os.path.join(dir_path, "docs", "tutorial-patchers")
    coll_output_path = os.path.join(tutorial_path, "FL_tutorial_names.txt")

    # If dir doesnt exist make, otherwise edit existing
    coll = open(coll_output_path, "w+")
    coll.truncate(0)

    # Find File Names
    all_files = os.listdir(
        tutorial_path
    )  # store all the file names of the tutorial path in an array

    # Only get the tutorial names
    tutorial_names = []

    # Have to do an additional loop here to sort all the tutorials alpha-numerically
    for item in all_files:
        if item.startswith("_"):
            tutorial_names.append(item)
    tutorial_names.sort(key=alphanum_key)

    # Write the contents of tutorial_names to the coll with some f strings
    idx = 0
    for item in tutorial_names:  # each item loop over all tutorial names list
        coll.write(f"{idx}, {item};\n")  # write that item into the buffer
        idx += 1

    coll.close()
