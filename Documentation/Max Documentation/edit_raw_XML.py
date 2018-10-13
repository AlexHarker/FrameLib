import os
import json
import xml.etree.ElementTree as et
from strippers import strip_extension



### directories ###
dir_path = os.path.dirname(os.path.realpath(__file__))
object_path = dir_path.replace('/Documentation/Max Documentation', '/FrameLib_Max_Objects')
raw_xml_path = f'{dir_path}/RawXML'
move_to_path = dir_path.replace('/Documentation/Max Documentation', '/Current Test Version/FrameLib/docs/refpages')
category_database_path = f'{dir_path}/category_database.json'

### Load the category_database.json ###
with open(category_database_path) as raw_json:
    category_database = json.load(raw_json)

raw_xml_list = os.listdir(raw_xml_path) #make a list with all the raw xml files in them

### Get category of file ###
def find_object_category(obj_string):
    for key in category_database:
        category_object_list = category_database[key]    
        for obj in category_object_list:
            if obj == obj_string:
                return [key, obj]

for i in range(len(raw_xml_list)):
    raw_xml_file_path = f'{raw_xml_path}/{raw_xml_list[i]}'
    obj_name = strip_extension(raw_xml_list[i], 2)
    tree = et.parse(raw_xml_file_path)
    root = tree.getroot()
    print(tree)             


### Edit that category into the relevant XML file ###









# for item in file_categories:
#     files = os.listdir(f'{object_path}/{item}')
#     if 'ibuffer' in files:
#         files.remove('ibuffer')
#     for i in range(len(files)):
#         files[i] = strip_extension(files[i], 1)
#     category_dict[item] = files
    
# with open(output_path, 'w+') as fp:
#     json.dump(category_dict, fp, indent=4)

