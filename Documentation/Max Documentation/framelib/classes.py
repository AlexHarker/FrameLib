import xml.etree.ElementTree as et
from pathlib import Path
from framelib.utils import read_json, write_json, strip_space, strip_extension
from framelib.variables import this_script
from shutil import copytree


class Documentation:
    """
    This class holds the relevant information about paths.
    By making it a class it makes it more flexible than hard coded values.
    """

    def __init__(self):
        self.this_script = this_script
        self.max_docs_dir = this_script.parents[1]
        self.repo_root = this_script.parents[3]

        # Import Max Paths
        self.package = self.repo_root / "Packaging" / "Max" / "FrameLib"

        # Temporary Directories
        self.temporary_dir = self.max_docs_dir / "__tmp__"
        self.databases_dir = self.temporary_dir / "db"
        self.raw_xml_dir = self.temporary_dir / "raw_xml"

        # Content Directory
        self.content_dir = self.max_docs_dir / "content"

        # Key Files
        self.category_database_path = self.databases_dir / "category_databases.json"
        self.object_relationships_path = self.content_dir / "object_relationships" / "object_relationships.json"

        self.set_max_paths()

        # Help Files
        self.help_dir = self.content_dir / "help_files"
        self.help_templates_dir = self.temporary_dir / "help_templates"

        # Manual XML
        self.manual_xml_dir = self.content_dir / "refpages"

        # The Max Objects Source Files
        self.source_path = self.repo_root / "FrameLib_Max_Objects"
        self.source_files = [x for x in self.source_path.rglob("fl.*.cpp")]

        # Read Object Info
        self.object_info = read_json(self.object_relationships_path)

        # Objects
        self.additional_valid_objects = [ "buffer~" ]

    def set_max_paths(self) -> None:
        self.refpages_dir = self.package / "docs" / "refpages"
        self.interfaces_dir = self.package / "interfaces"
        self.refpages_dir.mkdir(exist_ok=True, parents=True)
        self.interfaces_dir.mkdir(exist_ok=True, parents=True)

    def is_tutorial(self, name: str) -> bool:
        return "_" in name

    def refpage_name(self, obj_name: str) -> str:
        """Returns the refpage name of an object"""
        if self.is_tutorial(obj_name) or obj_name in self.additional_valid_objects:
            return obj_name
        file_name = obj_name + ".maxref.xml"
        ref_path = self.raw_xml_dir / file_name
        if not ref_path.exists():
            ref_path = self.manual_xml_dir / file_name
        if not ref_path.exists():
            ref_path = self.refpages_dir / "framelib-ref" / file_name
        return et.parse(ref_path).getroot().get("name")

    def seealso_aliased(self, object_name: str) -> dict:
        """Returns a seealso dict correctly aliased"""
        seealso = self.object_info[object_name]["seealso"][:]

        aliased = [self.refpage_name(x) for x in seealso]
        aliased.sort()

        return aliased



# A class to parse the XML files and build a JSON file from it #
class dParseAndBuild:
    """
    This class parses XML files related to Max objects, finds relevant information and then reformats it into a JSON file.
    """

    def __init__(self):
        self.tree = 0
        self.root = 0
        self.digest = ""
        self.d_master_dict = {}

    # Assign values to construct the json #
    def build_json_file(self):
        self.d_inner_data = {"digest": self.digest}
        self.d_outer_data = {self.object_name: self.d_inner_data}
        self.d_master_dict.update(self.d_outer_data)

    # Extract the info from the refpages #
    def extract_from_refpage(self, x):
        self.tree = et.parse(x)
        self.root = self.tree.getroot()  # c74object

        # Find Information #
        self.object_name = self.root.get("name")

        for child in self.root:
            if child.tag == "digest":
                self.digest = child.text

        # Strip whitespace #
        self.digest = strip_space(self.digest)

        # Call the build function #
        self.build_json_file()


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
        self.object_proper_name = strip_extension(x.stem, 1)  # get the object name (use the maxref name in case it is aliased)

        for child in self.root:
            if child.tag == "digest":
                self.digest = child.text

        # #strips whitespace from things
        self.digest = strip_space(self.digest)

    def extract_seealso(self, docs):
        """
        Extracts the see also contents from the master json file
        """
        self.seealso = docs.seealso_aliased(self.object_proper_name)
        for item in self.seealso:
            if docs.is_tutorial(item):
                self.seealso.remove(item)

    def extract_keywords(self, docs):
        """
        Extracts the keywords contents from the master json file
        """
        try:
            self.keywords = docs.object_info[self.object_proper_name]["keywords"]
        except KeyError:
            print(f"No keywords for {self.object_name}")


# A class to parse the XML files and build a JSON file from it #
class tParseAndBuild:
    """
    Takes info from the tutorial refpages FrameLib/docs/tutorials/framelib-tut/.
    Extracts the title, number and formats it into a .json.
    """

    def __init__(self):
        self.tree = 0
        self.root = 0
        self.difficulty = ""
        self.title = ""
        self.subtitle = ""
        self.description = ""
        self.d_master_dict = {}
        self.temp_list = []
        self.temp_outer_list = []

    # Extract the info from the refpages #
    def extract_from_refpage(self, x):
        self.tree = et.parse(x)
        self.root = self.tree.getroot()

        # create empty master dict and three dicts for different levels of difficulty
        self.d_skeleton = {}
        self.d_skeleton["Beginner"] = {}
        self.d_skeleton["Intermediate"] = {}
        self.d_skeleton["Advanced"] = {}

        # create empty inner dict #
        self.inner_dict = {}

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


class jParseAndBuild:
    """
    Performs a comprehensive parse of the .maxref files to interact with the umenu in real-time.
    The information is stored in a dict.
    """

    def __init__(self):
        self.tree = 0
        self.root = 0
        self.j_master_dict = {}

    def extract_from_refpage(self, x):
        self.tree = et.parse(x)
        self.root = self.tree.getroot()
        blank_param_dict = {}
        blank_internal = {}

        # Find Information

        self.object_name = strip_extension(x.stem, 1)  # get the object name (use the maxref name in case it is aliased)
        param_idx = 1  # reset a variable to track the parameter number
        for child in self.root:  # iterate over the sections
            if child.tag == "misc":  # if the section is misc
                if child.get("name") == "Parameters":  # if the section is misc and has name='Parameters'
                    for elem in child:  # for sub-sections
                        blank_internal = {"name": elem.get("name")}  # store the name with the key/pair 'name'

                        for subchild in elem:
                            if subchild.tag == "description":  # get the description out

                                # Format the parameter info for the helpfile

                                blank_desc = strip_space(subchild.text)
                                firstBullet = True;

                                for item in subchild:
                                    if item.tag == "bullet":  # if there are any bullet points
                                        if item.text != None:  # and its not none
                                            # if it is the first line of an enum it will be the title 'Parameter Options'
                                            if item.text[1] == "0":
                                                blank_desc += f"Parameter Options:"
                                                firstBullet = False

                                        # Deal with breaks in bullets

                                        for nested in item:
                                            if nested.tag == "br" or nested.tag == "p":
                                                nested.text = "\n\n    "
                                        if firstBullet:
                                            text = "".join(item.itertext());
                                            blank_desc += text
                                            firstBullet = False
                                        else:
                                            text = "".join(item.itertext());
                                            blank_desc += f"\n{text}"
                                    elif item.tag == "o":  # strip object tabs
                                        blank_desc += item.text
                                        blank_desc += item.tail.rstrip()
                                    elif item.tag == "br" or item.tag == "p":  # add line breaks
                                        if item.tail != None:
                                            blank_desc += "\n\n" + item.tail.rstrip()
                                        else:
                                            blank_desc += "\n\n"
                                        firstBullet = True
                                    elif item.tag == "h6":  # instantiation flag will be wrapped in a <h6>
                                            blank_desc += "".join(item.itertext())
                                            if item.tail.rstrip() != None:
                                                blank_desc += "\n\n" + item.tail.rstrip()
                                    elif item.tag == "i":  # default will be wrapped in a <i>
                                            blank_desc += "".join(item.itertext())
                                            if item.tail.rstrip() != None:
                                                blank_desc += item.tail.rstrip()

                        blank_internal["description"] = blank_desc  # set the description

                        # assign the blank_internal dict to a parameter number
                        blank_param_dict[param_idx] = blank_internal
                        param_idx += 1

        param_dict = {self.object_name: blank_param_dict}

        self.j_master_dict.update(param_dict)
