import xml.etree.ElementTree as et
import os
import yaml
from FrameLibDocs.strippers import strip_space
from FrameLibDocs.utils import write_json, cd_up, thin_list


class qParseAndBuild:
    """
    Performs a comprehensive parse of the .maxref files to interact with the umenu in real-time.
    The information is stored in a dict.
    """

    def __init__(self):
        self.tree = 0
        self.root = 0
        self.digest = "none"
        self.module = "none"
        self.category = "none"
        self.keywords = []
        self.seealso = []
        self.d_master_dict = {}
        self.object_name = str()

    def build_json_file(self):
        self.d_inner_data = dict(
            {
                "digest": self.digest,
                "module": "FrameLib",
                "category": ["FrameLib", self.category],
                "keywords": self.keywords,
                "seealso": self.seealso,
            }
        )
        self.d_outer_data = dict({self.object_name: self.d_inner_data})
        self.d_master_dict.update(self.d_outer_data)

    def extract_from_refpage(self, x):
        """
        x is a refpage.xml file that is parsed and has info extracted from it.
        """
        self.tree = et.parse(x)
        self.root = self.tree.getroot()

        # Find Information
        self.category = self.root.get("category")
        self.object_name = self.root.get("name")

        for child in self.root:
            if child.tag == "digest":
                self.digest = child.text

        # #strips whitespace from things
        self.digest = strip_space(self.digest)

    def extract_seealso(self, yaml):
        """
        Extracts the see also contents from the master yaml file
        """
        self.seealso = yaml[self.object_name]["seealso"]

    def extract_keywords(self, yaml):
        """
        Extracts the keywords contents from the master yaml file
        """
        self.keywords = yaml[self.object_name]["keywords"]


# ----------- THE GUTS ----------- #


def main(root):
    """
    Creates a dict for the Max Documentation system.
    This dict contains is essential for maxObjectLauncher/Refpages to pull the right info.

    Args:
        arg1: passes the root of the python files from the master script. Creates relative directories.
    """

    yaml_file = os.path.join(root, "object_relationships.yaml")
    object_info = None

    with open(yaml_file, "r") as stream:
        try:
            object_info = yaml.safe_load(stream)
        except yaml.YAMLError as exc:
            print(exc)

    bad_entries = [".DS_Store", "_c74_ref_modules.xml"]

    dir_path = root
    dir_path = os.path.join(cd_up(root, 2), "Current Test Version", "FrameLib")
    ref_dir = os.path.join(dir_path, "docs", "refpages")
    obj_lookup = os.path.join(dir_path, "interfaces", "FrameLib-obj-qlookup.json")

    worker = qParseAndBuild()

    # Make a list of file names and remove bad entries
    refpages = thin_list(os.listdir(ref_dir), bad_entries)

    # Check if any files were found and do your thing
    if refpages:
        for filename in refpages:
            current_category = filename
            source_file_name = os.path.join(ref_dir, filename)

            for filename in os.listdir(source_file_name):
                source_file = os.path.join(ref_dir, current_category, filename)
                worker.extract_from_refpage(source_file)
                worker.extract_keywords(object_info)
                worker.extract_seealso(object_info)
                worker.build_json_file()

            # Write out to JSON
        write_json(obj_lookup, worker.d_master_dict)
