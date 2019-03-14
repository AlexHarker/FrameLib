import json
import xml.etree.ElementTree as et
#from lxml import etree as et
import os
from strippers import strip_extension, strip_space

dir_path = os.path.dirname(os.path.realpath(__file__))
cpp_path = '/Users/jamesbradbury/FrameLib-fork/FrameLib_Max_Objects'
with open(f"{dir_path}/test_db.json") as f:
    category_db = json.load(f)
not_objects = []
finished_xml = []

# '/home/user/somefile'
# 1. iterate over xml files in a specific directory
# 2. Make each one a test value 
# 3. Run the set up function
# 4. Do the XML editing of seealsos

#### Find fl.~ xml files ####
for line in os.listdir(dir_path):
    if line.startswith('fl.') and line not in finished_xml :

        file_name = line
        framelib_name = strip_extension(line)

        print(finished_xml)
        for key in category_db:
            value = category_db[key]
            for i in range (len(value)):
                if value[i] == framelib_name:
                    object_array = category_db[key] #Store the category as an array so you can iterate over it
                    for i in range (len(object_array)):
                        if object_array[i] != framelib_name:
                            not_objects.append(object_array[i])

        #at this point you have an array of 'not' objects, a collection of names for see alsos which need to be put in the xml file
        #### XML STUFF ####
        #tree = et.parse(f"{dir_path}/{file_name}")
        #root = tree.getroot()
        #a = root.find('seealsolist')
        my_file = open(f"{dir_path}/{file_name}", 'r')
        lines_of_file = my_file.readlines()
        for lines in lines_of_file:
            lines = strip_space(lines)
            if lines == '<seealsolist>':
                pass
                #get the last line, and the line of seealso list then add one after

        # for item in not_objects:
        #     new_see_also = et.SubElement(a, 'seealso')
            #append item as an attribute name
        # tree.write(f"{dir_path}/{file_name}")
        finished_xml.append(file_name) #add the file name to the done list so it doenst accidentely loop over it again




# 		<seealso name="bach.expr" />
# 		<seealso name="bach.!-" />
# 		<seealso name="bach.+" />
# 		<seealso name="bach.*" />
# 		<seealso name="bach./" />
# 		<seealso name="-" />
# 		<seealso name="bach.mod-" />


# # Extract the info from the refpages #
# def extract_from_refpage(self, x):
#     self.tree = et.parse(x)
#     self.root = self.tree.getroot() #c74object

#     # Find Information #    
#     self.object_name = self.root.get('name') #finds the name so you don't have to do regex

#     for child in self.root:
#         if child.tag == 'digest':
#             self.digest = child.text

#     # Strip whitespace #
#     self.digest = strip_space(self.digest)

#     # Call the build function #
#     self.build_json_file()
