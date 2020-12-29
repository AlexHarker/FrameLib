import re
from FrameLibDocs.variables import current_version


# TODO cleanup this function and alphanum_key too
def try_integer(s) -> int:
    """Attempts to convert some input into an integer"""
    try:
        return int(s)
    except ValueError:
        return s


def alphanum_key(s: int):
    """ Turn a string into a list of string and number chunks."""
    return [try_integer(c) for c in re.split("([0-9]+)", s)]


def main():
    tutorial_path = current_version / "FrameLib" / "docs" / "tutorial-patchers"
    coll_output_path = tutorial_path / "fl-tutorial-names.txt"

    # If dir doesnt exist make, otherwise edit existing
    coll = open(coll_output_path, "w+")
    coll.truncate(0)

    # Find File Names
    all_files = [x.stem for x in tutorial_path.iterdir() if str(x.name).startswith("_")]
    all_files.sort(key=alphanum_key)

    # Write the contents of tutorial_names to the coll with some f strings
    idx = 0
    for item in all_files:  # each item loop over all tutorial names list
        coll.write(f"{idx}, {item};\n")  # write that item into the buffer
        idx += 1

    coll.close()

if __name__ == "__main__":
    main()