import json
import xml.etree.ElementTree as et
import os

dir_path = os.path.dirname(os.path.realpath(__file__))
ref_dir = dir_path + '/docs/refpages' # possibly make this relative
obj_lookup = dir_path + '/interfaces/FrameLib-obj-qlookup.json'

def strip_space(tostrip):
    tostrip = tostrip.lstrip()
    tostrip = tostrip.rstrip()
    return tostrip

class ParseAndBuild():
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

        print ('-' * 40)
        print('NAME: ', self.object_name)
        print('DIGEST: ', self.digest)
        print('SEEALSO: ', self.seealso_list)
        print('KEYWORDS: ', self.keywords)
        print('CATEGORY: ', self.category)
        print ('_' * 40)

# ----------- THE GUTS ----------- #
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
print('*' * 40)
print('Finished parsing and building object lookup file')
print(' ' * 40)










        







