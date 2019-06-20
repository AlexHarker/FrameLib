import os
import json
import xml.etree.ElementTree as et
import errno
from strippers import strip_extension
from helpers import cd_up, read_json, remove_ds

### This file should be probably be made into a class as its quite messy to define the find_object_category function inside of the main(). ###

def main(root):
    '''
    The purpose of this script is to set the categories for the Raw_XML files. C++ doesnt know about the categories and its easier to iterate file structures in python. So we do it here after the xml has been generated.
    It also copies the xml files to the refpages directory.
    '''
    ### directories ###
    dir_path = root
    raw_xml_path = os.path.join(dir_path, 'tmp')
    move_to_path = os.path.join(cd_up(root, 2), 'Current Test Version', 'FrameLib', 'docs', 'refpages')
    category_database_path = os.path.join(dir_path, 'category_database.json')

    ### Load the category_database.json ###
    category_database = read_json(category_database_path)
    
    ### Get category of file ###
    def find_object_category(obj_string):
        '''
        Finds the dictionary name for an object inside of that dictionary.
        Is used to work backwards and find which category each object belongs to.
        '''
        for key in category_database:
            category_object_list = category_database[key]    
            for obj in category_object_list:
                if obj == obj_string:
                    return key
    try:
        raw_xml_list = remove_ds(os.listdir(raw_xml_path)) #make a list with all the raw xml files in them
    except FileNotFoundError:
        print('Unable to find any xml files to parse. Moving on without parsing object references.')

    try:
        raw_xml_list
    except NameError:
        pass
    else:
        for i in range(len(raw_xml_list)):
            raw_xml_file_path = os.path.join(raw_xml_path, raw_xml_list[i]) #make a raw file path to load the list somewhere
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

            tree.write(os.path.join(move_to_path, category, raw_xml_list[i])) # write out to new XML
