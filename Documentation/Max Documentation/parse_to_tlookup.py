# imports #
import json
import xml.etree.ElementTree as et
import os
from strippers import strip_space

# A class to parse the XML files and build a JSON file from it #
class ParseAndBuild():
    def __init__(self):
        self.tree = 0
        self.root = 0
        self.difficulty = 'none'
        self.title = 'none'
        self.subtitle = 'none'
        self.description = 'none'
        self.d_master_dict = dict({})
        self.temp_list = []
        self.temp_outer_list = []

    # Extract the info from the refpages #
    def extract_from_refpage(self, x):
        self.tree = et.parse(x)
        self.root = self.tree.getroot()

        # create empty master dict #
        self.d_skeleton = dict({})
        self.d_skeleton["Beginner"] = dict({})
        self.d_skeleton["Intermediate"] = dict({})
        self.d_skeleton["Advanced"] = dict({})

        # create empty inner dict #
        self.inner_dict = dict({})
        
        # Tags are the attributes stored inline, the text is the white #
        for child in self.root:
            self.counter = 0
            for elem in child:
                if elem.tag == 'ul':
                    for li in elem:                 
                        for title in li:
                            self.inner_dict.clear() # wipe the inner dictionary
                            self.inner_dict[title.text] = title.get('digest') #create an inner dict with the tutorial and info
                            self.d_skeleton[title.get('diff')].update(self.inner_dict) # append inner dict to right difficulty dict
    
# ----------- THE GUTS ----------- #
def main(root):

    # Directory stuff #
    dir_path = root
    dir_path = dir_path.replace('/Documentation/Max Documentation', '/Current Test Version/FrameLib')
    ref_dir = dir_path + '/docs/tutorials/FrameLib-tut/00_fl_index.maxtut.xml' 
    obj_lookup = dir_path + '/interfaces/FrameLib-obj-tlookup.json'

    worker = ParseAndBuild()

    worker.extract_from_refpage(ref_dir)

    with open(obj_lookup, 'w') as fp:
        json.dump(worker.d_skeleton, fp, indent=4)










        








