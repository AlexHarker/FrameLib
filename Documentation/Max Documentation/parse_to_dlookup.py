# imports #
import json
import xml.etree.ElementTree as et
import os
from strippers import strip_space

# Directory stuff #
dir_path = os.path.dirname(os.path.realpath(__file__))
ref_dir = dir_path + '/docs/refpages'
obj_lookup = dir_path + '/interfaces/FrameLib-obj-dlookup.json'


# A class to parse the XML files and build a JSON file from it #
class ParseAndBuild():
    def __init__(self):
        self.tree         = 0
        self.root         = 0
        self.digest       = 'none'
        self.d_master_dict = dict({})
    
    # Assign values to construct the json #
    def build_json_file(self):
        self.d_inner_data = dict({
            'digest' : self.digest
        })
        self.d_outer_data = dict({self.object_name:self.d_inner_data})
        self.d_master_dict.update(self.d_outer_data)

    # Extract the info from the refpages #
    def extract_from_refpage(self, x):
        self.tree = et.parse(x)
        self.root = self.tree.getroot() #c74object

        # Find Information #    
        self.object_name = self.root.get('name') #finds the name so you don't have to do regex

        for child in self.root:
            if child.tag == 'digest':
                self.digest = child.text

        # Strip whitespace #
        self.digest = strip_space(self.digest)

        # Call the build function #
        self.build_json_file()

# ----------- THE GUTS ----------- #
def main():
    worker = ParseAndBuild()

    for filename in os.listdir(ref_dir):
        if filename != '.DS_Store':
            if filename != '_c74_ref_modules.xml':
                current_category = filename
                source_file_name = ref_dir + '/' + filename

        for filename in os.listdir(source_file_name):
            if filename != '.DS_Store':
                source_file = ref_dir + '/' + current_category + '/' + filename
                worker.extract_from_refpage(source_file)

    with open(obj_lookup, 'w') as fp:
        json.dump(worker.d_master_dict, fp, indent=4)










        








