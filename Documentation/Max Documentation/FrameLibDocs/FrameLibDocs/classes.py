# A class to parse the XML files and build a JSON file from it #
class dParseAndBuild:
    """
    This class parses XML files related to Max objects, finds relevant information and then reformats it into a JSON file.
    """

    def __init__(self):
        self.tree = 0
        self.root = 0
        self.digest = ""
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


# A class to parse the XML files and build a JSON file from it #
class tParseAndBuild:
    """
    Takes info from the tutorial refpages /FrameLib/Current Test Version/FrameLib/docs/tutorials/FrameLib-tut/.
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


class jParseAndBuild:
    """
    Performs a comprehensive parse of the .maxref files to interact with the umenu in real-time.
    The information is stored in a dict.
    """

    def __init__(self):
        self.tree = 0
        self.root = 0
        self.j_master_dict = dict({})

    def extract_from_refpage(self, x):
        self.tree = et.parse(x)
        self.root = self.tree.getroot()
        blank_param_dict = {}
        blank_internal = {}
        blank_descr = ""
        enums = ""

        # Find Information
        self.object_name = self.root.get("name")  # get the object name
        param_idx = 1  # reset a variable to track the parameter number
        for child in self.root:  # iterate over the sections
            if child.tag == "misc":  # if the section is misc
                if (
                    child.get("name") == "Parameters"
                ):  # if the section is misc and has name='Parameters'
                    for elem in child:  # for sub-sections
                        blank_internal = {
                            "name": elem.get("name")
                        }  # store the name with the key/pair 'name'

                        for description in elem:  # get the description out
                            blank_desc = strip_space(description.text)

                            #### TODO ####
                            # Edit the description if its multiline as is the case with fl.map~ #
                            ##############

                            for bullet in description:  # if there are any bullet points
                                if bullet.text != None:  # and its not none
                                    if (
                                        bullet.text[1] == "0"
                                    ):  # if it is the first line it will be the title 'Parameter Options'
                                        blank_desc += f"\n\nParameter Options:"

                                    blank_desc += f"\n{bullet.text}"
                        blank_internal[
                            "description"
                        ] = blank_desc  # set the description

                        blank_param_dict[
                            param_idx
                        ] = (
                            blank_internal
                        )  # assign the blank_internal dict to a parameter number
                        param_idx += 1

        param_dict = dict({self.object_name: blank_param_dict})

        self.j_master_dict.update(param_dict)
