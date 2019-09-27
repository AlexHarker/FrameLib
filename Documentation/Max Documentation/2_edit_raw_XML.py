import os
import json
import xml.etree.ElementTree as et
import errno
import yaml
from FrameLibDocs.strippers import strip_extension
from FrameLibDocs.utils import cd_up, read_json, remove_ds, get_path, read_yaml

def indent(elem, level=0):
    i = "\n" + level * "  "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
        for elem in elem:
            indent(elem, level + 1)
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i

def main(root):
    """
    The purpose of this script is to set the categories for the Raw_XML files. C++ doesnt know about the categories and its easier to iterate file structures in python. So we do it here after the xml has been generated.
    It also copies the xml files to the refpages directory.
    """


    # directories
    dir_path = root
    raw_xml_path = os.path.join(dir_path, "__tmp__", "raw_xml")
    move_to_path = os.path.join(cd_up(root, 2), "Current Test Version", "FrameLib", "docs", "refpages")
    category_database_path = os.path.join(dir_path, "__tmp__", "db", "category_database.json")
    yaml_file = os.path.join(root, "object_relationships.yaml")
    object_info = read_yaml(yaml_file)

    # Load the category_database.json
    category_database = read_json(category_database_path)

    # Get category of file
    def find_object_category(obj_string):
        """
        Finds the dictionary name for an object inside of that dictionary.
        Is used to work backwards and find which category each object belongs to.
        """
        for key in category_database:
            category_object_list = category_database[key]
            for obj in category_object_list:
                if obj == obj_string:
                    return key

    try:
        raw_xml_list = remove_ds(os.listdir(raw_xml_path))  # make a list with all the raw xml files in them
    except FileNotFoundError:
        print("Unable to find any xml files to parse. Moving on without parsing object references.")
    else:
        for raw_xml in raw_xml_list:
            obj_name = strip_extension(raw_xml, 2)  # just get the file name
            category = find_object_category(obj_name)  # get the category of the object name
            tree = et.parse(os.path.join(raw_xml_path, raw_xml))  # parse the xml file
            root = tree.getroot()  # get root and assign to root var
            root.set("category", category)  # set category attribute of root to the category found in json


            # This replaces the meta data tag. It produces a lot of errors which are filtered by the try/except structure but it should be changed to something else #
            if category != None:
                for elem in root.getiterator():  # for all the elements in the root of the xml tree
                    try:
                        elem.text = elem.text.replace("!@#@#$", category)  # try to replace specific text with category found in json
                    except AttributeError:
                        pass  # else pass because it will throw some errors
                        # print('Error trying to replace category string.')
                        
            if not os.path.exists(os.path.join(move_to_path, category)):
                try:
                    os.makedirs(os.path.join(move_to_path))
                except OSError as e:
                    if e.errno != errno.EEXIST:
                        raise  # if directory is made between os.path.exists and os.makedirs calls this will fail with an OSError. This raises an error to warn the user rather than pushing on

            # Create seealso and keywords
            details = object_info[obj_name]
            for elem in root:
                if elem.tag == "seealsolist":
                    for seealso in details["seealso"]:
                        new_element = et.Element("seealso")
                        new_element.set("Name", seealso)
                        elem.append(new_element)
                if elem.tag == "misc" and elem.attrib["name"] == "Discussion":
                    for sub_elem in elem:
                        if (
                            sub_elem.tag == "entry"
                            and sub_elem.attrib["name"] == "Keywords"
                        ):
                            for desc in sub_elem:
                                temp_string = ",".join(details["keywords"])
                                desc.text = temp_string
            # Pretty Print #
            indent(root)
            out_path = os.path.join(move_to_path, category, raw_xml)
            try:
                os.makedirs(os.path.join(move_to_path, category))
            except OSError as e:
                if e.errno != errno.EEXIST:
                    raise
            tree.write(out_path)  # write out to new XML
