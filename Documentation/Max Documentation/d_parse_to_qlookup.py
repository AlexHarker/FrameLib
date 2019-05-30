import json
import xml.etree.ElementTree as et
import os
from strippers import strip_space

class qParseAndBuild():
    '''
    Performs a comprehensive parse of the .maxref files to interact with the umenu in real-time.
    The information is stored in a dict.
    '''

    def __init__(self):
        self.tree         = 0
        self.root         = 0
        self.digest       = 'none'
        self.module       = 'none'
        self.category     = 'none'
        self.keywords     = []
        self.seealso_list = []
        self.d_master_dict = dict({})
    
    def build_json_file(self):
        self.d_inner_data = dict({
            'digest' : self.digest,
            'module' : 'FrameLib',
            'category' : ['FrameLib', self.category],
            'keywords' : self.keywords,
            'seealso' : self.seealso_list
        })
        self.d_outer_data = dict({self.object_name:self.d_inner_data})
        self.d_master_dict.update(self.d_outer_data)

    def extract_from_refpage(self, x):
        self.tree = et.parse(x)
        self.root = self.tree.getroot() #c74object

        # Find Information #    
        self.category = self.root.get('category') #finds the category 
        self.object_name = self.root.get('name') #finds the name so you don't have to do regex

        for child in self.root:
            if child.tag == 'digest':
                self.digest = child.text
            elif child.tag == 'seealsolist':
                self.seealso_list.clear()
                for seealso in child:
                    self.temp_seealso = seealso.get('name')
                    self.seealso_list.append(self.temp_seealso)
            elif child.tag == 'misc':
                self.temp_test = child.get('name')
                if self.temp_test == 'Discussion':
                    for entry in child:
                        for description in entry:
                            self.keywords = description.text

        # #strips whitespace from things
        self.digest = strip_space(self.digest)
        self.keywords = strip_space(self.keywords)
        self.keywords = self.keywords.split(',')
        self.keywords = [x.strip(' ') for x in self.keywords]

        self.build_json_file()

# ----------- THE GUTS ----------- #

def main(root):
    '''
    Creates a dict for the Max Documentation system.
    This dict contains more detailed information displayed in real-time when hovering over a certain tutorial in the umenu.

    Args:
        arg1: passes the root of the python files from the master script. Creates relative directories.
    '''

    bad_entries = ['.DS_Store', '_c74_ref_modules.xml']

    dir_path = root
    dir_path = dir_path.replace('/Documentation/Max Documentation', '/Current Test Version/FrameLib')
    ref_dir = f'{dir_path}/docs/refpages' 
    obj_lookup = f'{dir_path}/interfaces/FrameLib-obj-qlookup.json'

    worker = qParseAndBuild()
    
    # Make a list of file names and remove bad entries
    refpages = os.listdir(ref_dir)
    for badness in bad_entries:
        if badness in refpages:
            refpages.remove(badness)

    # Check if any files were found and do your thing
    if refpages:  
        for filename in refpages:
            current_category = filename
            source_file_name = f'{ref_dir}/{filename}'

            for filename in os.listdir(source_file_name):
                if filename != '.DS_Store':
                    source_file = f'{ref_dir}/{current_category}/{filename}'
                    worker.extract_from_refpage(source_file)

        with open(obj_lookup, 'w') as fp:
            json.dump(worker.d_master_dict, fp, indent=4)











        








