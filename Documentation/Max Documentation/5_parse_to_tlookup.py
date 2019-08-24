import json
import os
import xml.etree.ElementTree as et
from FrameLibDocs.strippers import strip_space
from FrameLibDocs.utils import write_json, cd_up

# A class to parse the XML files and build a JSON file from it #
class tParseAndBuild:
    """
    Takes info from the tutorial refpages /FrameLib/Current Test Version/FrameLib/docs/tutorials/FrameLib-tut/.
    Extracts the title, number and formats it into a .json.
    """

    def __init__(self):
        self.tree = 0
        self.root = 0
        self.difficulty = "none"
        self.title = "none"
        self.subtitle = "none"
        self.description = "none"
        self.d_master_dict = dict({})
        self.temp_list = []
        self.temp_outer_list = []

    # Extract the info from the refpages #
    def extract_from_refpage(self, x):
        self.tree = et.parse(x)
        self.root = self.tree.getroot()

        # create empty master dict and three dicts for different levels of difficulty
        self.d_skeleton = dict({})
        self.d_skeleton["Beginner"] = dict({})
        self.d_skeleton["Intermediate"] = dict({})
        self.d_skeleton["Advanced"] = dict({})

        # create empty inner dict #
        self.inner_dict = dict({})

        # Tags are the attributes stored inline, the text is the white #
        for child in self.root:
            for elem in child:
                if elem.tag == "ul":
                    for li in elem:
                        for title in li:
                            self.inner_dict.clear()  # wipe the inner dictionary
                            self.inner_dict[title.text] = title.get(
                                "digest"
                            )  # create an inner dict with the tutorial and info
                            self.d_skeleton[title.get("diff")].update(
                                self.inner_dict
                            )  # append inner dict to the right difficulty dict


# ----------- THE GUTS ----------- #
def main(root):
    """
    Creates tutorial information and stores it in a dictionary format. 
    This information is displayed to the user in a umenu.

    Args:
        arg1: passes the root of the python files from the master script. Creates relative directories.
    """

    # Directory stuff #
    dir_path = root
    dir_path = os.path.join(cd_up(root, 2), "Current Test Version", "FrameLib")
    ref_dir = os.path.join(
        dir_path, "docs", "tutorials", "FrameLib-tut", "00_fl_index.maxtut.xml"
    )
    obj_lookup = os.path.join(dir_path, "interfaces", "FrameLib-obj-tlookup.json")

    worker = tParseAndBuild()

    worker.extract_from_refpage(ref_dir)

    write_json(obj_lookup, worker.d_skeleton)
