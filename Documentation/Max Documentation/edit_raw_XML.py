import os
import json
import xml.etree.ElementTree as et
import errno
from strippers import strip_extension


### This file should be probably be made into a class as its quite messy to define the find_object_category function inside of the main(). ###

def main():
    ### directories ###
    dir_path = os.path.dirname(os.path.realpath(__file__))
    object_path = dir_path.replace('/Documentation/Max Documentation', '/FrameLib_Max_Objects')
    raw_xml_path = f'{dir_path}/RawXML'
    move_to_path = dir_path.replace('/Documentation/Max Documentation', '/Current Test Version/FrameLib/docs/refpages')
    category_database_path = f'{dir_path}/category_database.json'

    ### Load the category_database.json ###
    with open(category_database_path) as raw_json:
        category_database = json.load(raw_json)
    
    ### Get category of file ###
    def find_object_category(obj_string):
        for key in category_database:
            category_object_list = category_database[key]    
            for obj in category_object_list:
                if obj == obj_string:
                    return key

    raw_xml_list = os.listdir(raw_xml_path) #make a list with all the raw xml files in them

    ### Complex find and replace ###
    for i in range(len(raw_xml_list)):
        if raw_xml_list[i] != '.DS_Store': #filter out annoying hidden files
            raw_xml_file_path = f'{raw_xml_path}/{raw_xml_list[i]}' #make a raw file path to load the list somewhere
            obj_name = strip_extension(raw_xml_list[i], 2) #just get the file name
            category = find_object_category(obj_name) #get the category of the object name
            tree = et.parse(raw_xml_file_path) #parse the xml file
            root = tree.getroot() #get root and assign to root var
            root.set('category', category) #set category attribute of root to the category found in json
            ### This replaces the meta data tag. It produces a lot of errors which are filtered by the try/except structure but it should be changed to something else ###
            if category != None:
                for elem in root.getiterator(): #for all the elements in the root of the xml tree
                    try:
                        elem.text = elem.text.replace('!@#@#$', category) #try to replace specific text with category found in json
                    except AttributeError:
                        pass #else pass because it will throw some errors
            if not os.path.exists(f'{move_to_path}/{category}'):
                try:
                    os.makedirs(f'{move_to_path}/{category}')
                except OSError as e:
                    if e.errno != errno.EEXIST:
                        raise # if directory is made between os.path.exists and os.makedirs calls this will fail with an OSError. This raises an error to warn the user rather than pushing on

            tree.write(f'{move_to_path}/{category}/{raw_xml_list[i]}') #write the xml file out
