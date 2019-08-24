import json
import xml.etree.ElementTree as et
import os
from FrameLibDocs.strippers import strip_space
from FrameLibDocs.utils import cd_up, write_json, remove_ds

# A class to parse the XML files and build a JSON file from it #
class dParseAndBuild:
    """
    This class parses XML files related to Max objects, finds relevant information and then reformats it into a JSON file.
    """

    def __init__(self):
        self.tree = 0
        self.root = 0
        self.digest = "none"
        self.d_master_dict = dict({})

    # Assign values to construct the json #
    def build_json_file(self):
        self.d_inner_data = dict({"digest": self.digest})
        self.d_outer_data = dict({self.object_name: self.d_inner_data})
        self.d_master_dict.update(self.d_outer_data)

    # Extract the info from the refpages #
    def extract_from_refpage(self, x):
        self.tree = et.parse(x)
        self.root = self.tree.getroot()  # c74object

        # Find Information #
        self.object_name = self.root.get(
            "name"
        )  # finds the name so you don't have to do regex

        for child in self.root:
            if child.tag == "digest":
                self.digest = child.text

        # Strip whitespace #
        self.digest = strip_space(self.digest)

        # Call the build function #
        self.build_json_file()


# ----------- THE GUTS ----------- #
def main(root):
    """
    A simplified version of the qlookup used to display information about specific objects when hovered over in the umenu.
    The qlookup might replace this entire script.

    Args:
        arg1: passes the root of the python files from the master script. Creates relative directories.
    """
    bad_entries = [".DS_Store", "_c74_ref_modules.xml"]
    # Directory stuff #
    dir_path = root
    dir_path = os.path.join(cd_up(root, 2), "Current Test Version", "FrameLib")
    ref_dir = os.path.join(dir_path, "docs", "refpages")
    obj_lookup = os.path.join(dir_path, "interfaces", "FrameLib-obj-dlookup.json")

    worker = dParseAndBuild()

    # Make a list of file names and remove bad entries
    refpages = remove_ds(os.listdir(ref_dir))
    for badness in bad_entries:
        if badness in refpages:
            refpages.remove(badness)

    # Check if any files were found and do your thing
    if refpages:
        for filename in refpages:
            current_category = filename
            source_file_name = os.path.join(ref_dir, filename)

            for filename in os.listdir(source_file_name):
                source_file = os.path.join(ref_dir, current_category, filename)
                worker.extract_from_refpage(source_file)

        write_json(obj_lookup, worker.d_master_dict)
    else:
        print("Found no XML files to parse. Moving on to next stage.")
