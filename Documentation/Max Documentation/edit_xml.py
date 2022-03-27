import os, json, re
import xml.etree.ElementTree as et
from framelib.utils import read_json, strip_extension
from framelib.classes import Documentation


def indent(elem, level=0):
    """
    Although the XML will still be valid after processing it won't have ANY indentation.
    This function adds indentation to make it more humanly readable.
    """
    i = "\n" + level * "  "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
        if elem.tag != "description":
            for child_elem in elem:
                indent(child_elem, level + 1)
        if not elem[len(elem) - 1].tail or not elem[len(elem) - 1].tail.strip():
            elem[len(elem) - 1].tail = i
    else:
        if level and not elem.tail or not elem.tail.strip():
            elem.tail = i


def main(docs):
    print(docs.refpages_dir)
    """
    The purpose of this script is to set the categories for the Raw_XML files.
    C++ doesnt know about the categories and its easier for me to iterate file structures in Python.
    It also copies the xml files to the refpages directory after they're processed.
    """

    object_info = read_json(docs.object_relationships_path)
    category_database = read_json(docs.category_database_path)

    # Get category of file
    def find_object_category(obj_string: str) -> str:
        """
        Finds the dictionary name for an object inside of that dictionary.
        Works backwards and find which category each object belongs to.
        """
        for key in category_database:
            category_object_list = category_database[key]
            for obj in category_object_list:
                if obj == obj_string:
                    return key

    raw_xml_list = [x for x in docs.raw_xml_dir.rglob("fl.*.xml")]
    manual_xml_list = [x for x in docs.manual_xml_dir.rglob("fl.*.xml")]
    print(f"Found {len(raw_xml_list)} .xml files to process.")
    docs.refpages_dir.mkdir(exist_ok=True)

    for raw_xml in raw_xml_list:
        obj_name = strip_extension(raw_xml.name, 2)  # just get the file name
        category = find_object_category(obj_name)  # get the category of the object name
        tree = et.parse(raw_xml)  # parse the xml file

        root = tree.getroot()  # get root and assign to root var
        root.set("category", category)  # set category attribute of root to the category found in json

        for i in root.iter():
            # Replace category
            try:
                i.text = i.text.replace("!@#@#$", category)
            except AttributeError:
                pass

        # Create seealso and keywords
        try:
            details = object_info[obj_name]
        except KeyError:
            print(f'No entry for {raw_xml} in object_relationships.json')
        else:
            for elem in root:
                if elem.tag == "seealsolist":
                    try:
                        for seealso in details["seealso"]:
                            new_element = et.Element("seealso")
                            new_element.set("name", seealso)
                            elem.append(new_element)
                    except KeyError:
                        print(f"No seealso for {obj_name}")
                if elem.tag == "misc" and elem.attrib["name"] == "Discussion":
                    for sub_elem in elem:
                        if sub_elem.tag == "entry" and sub_elem.attrib["name"] == "Keywords":
                            for desc in sub_elem:
                                temp_string = ", ".join(details["keywords"])
                                desc.text = temp_string
        # Pretty Print
        indent(root)

        # Write out
        xml_second_pass = docs.temporary_dir / "raw_xml_2"
        xml_second_pass.mkdir(exist_ok=True)

        unescaped_file = xml_second_pass / raw_xml.name
        tree.write(unescaped_file)

        docs.refpages_dir.mkdir(exist_ok=True)
        refpages_parent = docs.refpages_dir / "framelib-ref"
        refpages_parent.mkdir(exist_ok=True)
        final_path = refpages_parent / raw_xml.name
        final_file = open(final_path, "w+")
        final_file.write("<?xml version='1.0' encoding='utf-8' standalone='yes'?>\n")
        final_file.write("<?xml-stylesheet href='./_c74_ref.xsl' type='text/xsl'?>\n")
        with open(unescaped_file, "r") as f:
            xml = f.read()
            for line in xml:
                final_file.write(line)
            final_file.close()

    for manual_xml in manual_xml_list:
        refpages_parent = docs.refpages_dir / "framelib-ref"
        refpages_parent.mkdir(exist_ok=True)
        final_path = refpages_parent / manual_xml.name
        final_file = open(final_path, "w+")
        with open(manual_xml, "r") as f:
            xml = f.read()
            for line in xml:
                final_file.write(line)
            final_file.close()

if __name__ == "__main__":
    main(Documentation())
