import json
import xml.etree.ElementTree as et
import os
from strippers import strip_space
from helpers import write_json, cd_up

class jParseAndBuild():
    '''
    Performs a comprehensive parse of the .maxref files to interact with the umenu in real-time.
    The information is stored in a dict.
    '''

    def __init__(self):
        self.tree         = 0
        self.root         = 0
        self.j_master_dict = dict({})

    def extract_from_refpage(self, x):
        self.tree = et.parse(x)
        self.root = self.tree.getroot() # c74object
        blank_param_dict = {}
        blank_internal = {}
        blank_descr = ''
        enums = ''

        # Find Information #    
        self.object_name = self.root.get('name') # get the object name
        param_idx = 0 # reset a variable to track the parameter number
        for child in self.root: # iterate over the sections
            if child.tag == 'misc': # if the section is misc
                if child.get('name') == 'Parameters': # if the section is misc and has name='Parameters'
                    for elem in child: # for sub-sections
                        blank_internal = {'name' : elem.get('name')} # store the name with the key/pair 'name'

                        for description in elem: # get the description out
                            blank_desc = strip_space(description.text)
                            
                            for bullet in description: # if there are any bullet points
                                if bullet.text != None: # and its not none
                                    if bullet.text[1] == '0':
                                        blank_desc += f'\n\nParameter Options:'

                                    blank_desc += f'\n{bullet.text}'
                        blank_internal['description'] = blank_desc # set the description
                        blank_param_dict[param_idx] = blank_internal # assign the blank_internal dict to a parameter number
                        param_idx += 1

        param_dict = dict({self.object_name:blank_param_dict})

        self.j_master_dict.update(param_dict)
        
        # once we've made the param stuff we append it to a dict with the name of the object
        # 

# ----------- THE GUTS ----------- #

def main(root):
    '''
    Creates a dict for the Max Documentation system.
    This dict contains more detailed information displayed in real-time when hovering over a certain tutorial in the umenu.

    Args:
        arg1: passes the root of the python files from the master script. Creates relative directories.
    '''
    # paths are determined relatively.
    bad_entries = ['.DS_Store', '_c74_ref_modules.xml']

    dir_path = root
    dir_path = os.path.join(cd_up(root, 2), 'Current Test Version', 'FrameLib')
    ref_dir = os.path.join(dir_path, 'docs', 'refpages')
    obj_lookup = os.path.join(dir_path, 'interfaces', 'FrameLib-obj-jlookup.json')

    worker = jParseAndBuild() # make an instance of the class
    
    # Make a list of file names and remove bad entries
    refpages = os.listdir(ref_dir)
    for badness in bad_entries:
        if badness in refpages:
            refpages.remove(badness)

    # Check if any files were found and do your thing
    if refpages:  
        for filename in refpages:
            current_category = filename
            source_file_name = os.path.join(ref_dir, filename)

            for filename in os.listdir(source_file_name):
                if filename != '.DS_Store': 
                    source_file = os.path.join(ref_dir, current_category, filename)
                    worker.extract_from_refpage(source_file)
                    
        write_json(obj_lookup, worker.j_master_dict)










        








