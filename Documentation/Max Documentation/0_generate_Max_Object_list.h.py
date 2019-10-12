import os
import re
import sys
from FrameLibDocs.utils import get_path, cd_up, strip_space, strip_extension

ignored_objects = []
for arg in sys.argv:
    ignored_objects.append(arg)
ignored_objects = ignored_objects[1:]

root = cd_up(get_path(), 2)

# Create the Max_Object_list.h and add skeleton
op = open(os.path.join(root, "Max_Object_List.h"), "w+")
op.write('#include "FrameLib_TypeList.h"')
op.write("\n \n")
op.write("using FrameLib_DSPList = detail::FrameLib_Typelist<")
op.write("\n \n")

# Directory formation
max_source_folder = os.path.join(cd_up(root, 2), "FrameLib_Max_Objects")
# A list of the categories. Is used to find all the source files.
max_source_categories = os.listdir(max_source_folder)

# Try removing unnecessary stuff, otherwise throw some info that it was not there
try:
    max_source_categories.remove("_MaxSDK_")
except ValueError:
    print("No _MaxSDK_ to delete")
    pass

try:
    max_source_categories.remove(".DS_Store")
except ValueError:
    print("No .DS_Store")
    pass

try:
    max_source_categories.remove("Common")
except ValueError:
    print("No common folder")
    pass

max_source_categories.sort()  ## Sort it so that its in an order

## Get folders in the parent Max Objects Folder
## get total amount of files
source_file_list = []
for folder in max_source_categories:
    ## Make var containing the name of the folder for a given category
    category_folder = os.path.join(max_source_folder, folder)
    ## Make a list containing all of the source files in a single category
    file_list = os.listdir(category_folder)
    ## Get rid of ibuffer file when traversing to buffer ateogry
    if "ibuffer" in file_list:
        file_list.remove("ibuffer")

    if ".DS_Store" in file_list:
        file_list.remove(".DS_Store")

    for k in ignored_objects:
        try:
            file_list.remove(f"{k}.cpp")
        except:
            pass

    for j in file_list:
        source_file_list.append([os.path.join(category_folder), j])


def write_comma(counter, ceiling):
    if counter < ceiling - 1:
        op.write(",")
        op.write("\n")
    else:
        op.write("\n")


# Recreate full paths to open and parse for type cases
counter = 0
for category_folder, name in source_file_list:
    with open(os.path.join(category_folder, name), "r") as cpp:
        # flatten it with no spaces whatsoever
        source_file = cpp.read().replace("\n", "").replace(" ", "")
        search_area = source_file.split('extern"C"intC74_EXPORTmain(void){')[1]
        # Do this just before info gets cleaved
        fl_object_name = search_area.split("<")[1]
        fl_object_name = fl_object_name.split(">")[0]
        # if it contains ,kAllInputs remove
        if "," in fl_object_name:
            fl_object_name = fl_object_name.split(",")[0]
        # a bit more pruning
        if fl_object_name == "FrameLib_MaxClass_ToMax":
            fl_object_name = "FrameLib_ToHost"
        elif fl_object_name == "FrameLib_MaxClass_FromMax":
            fl_object_name = "FrameLib_FromHost"
        elif fl_object_name == "FrameLib_MaxClass_Info":
            fl_object_name = "FrameLib_Info"
        elif fl_object_name == "FrameLib_MaxClass_Read":
            fl_object_name = "FrameLib_Read"
        elif fl_object_name == "FrameLib_MaxClass_ComplexExpression":
            fl_object_name = "FrameLib_ComplexExpression"
        elif fl_object_name == "FrameLib_MaxClass_Expression":
            fl_object_name = "FrameLib_Expression"
        ## Now limit search area
        search_area = search_area.split("<")[0]

        # infer type with brutal checking by looking at text in the extern bit (search area)
        if "_Expand" in search_area:
            op.write(f"FrameLib_Expand<{fl_object_name}>")
            ## Condition to check if its the last one
            write_comma(counter, len(source_file_list))
        elif "_Expand" not in search_area and "makeClass" in search_area:
            op.write(f"FrameLib_Expand<{fl_object_name}>")
            ## Condition to check if its the last one
            write_comma(counter, len(source_file_list))
        elif "_Expand" not in search_area and "makeClass" not in search_area:
            op.write(fl_object_name)
            write_comma(counter, len(source_file_list))
        counter += 1

## Demarcate end of this section
op.write("\n \n")
op.write(">;")
op.write("\n \n")

## Start const bit
for category_folder, name in source_file_list:

    with open(os.path.join(category_folder, name), "r") as cpp:
        source_file = (
            cpp.read().replace("\n", "").replace(" ", "")
        )  # flatten it with no spaces whatsoever
        search_area = source_file.split('extern"C"intC74_EXPORTmain(void){')[1]

        ## Do this just before info gets cleaved
        fl_object_name = search_area.split("<")[1]
        fl_object_name = fl_object_name.split(">")[0]
        ## if it contains ,kAllInputs remove
        if "," in fl_object_name:
            fl_object_name = fl_object_name.split(",")[0]
        ## a bit more pruning
        if fl_object_name == "FrameLib_MaxClass_ToMax":
            fl_object_name = "FrameLib_ToHost"
        elif fl_object_name == "FrameLib_MaxClass_FromMax":
            fl_object_name = "FrameLib_FromHost"
        elif fl_object_name == "FrameLib_MaxClass_Info":
            fl_object_name = "FrameLib_Info"
        elif fl_object_name == "FrameLib_MaxClass_Read":
            fl_object_name = "FrameLib_Read"
        elif fl_object_name == "FrameLib_MaxClass_ComplexExpression":
            fl_object_name = "FrameLib_ComplexExpression"
        elif fl_object_name == "FrameLib_MaxClass_Expression":
            fl_object_name = "FrameLib_Expression"
        ## Now limit search area
        search_area = search_area.split("<")[0]
        no_ext = os.path.splitext(name)[0]
        no_ext = no_ext.split("~")[0]
        # infer type with brutal checking by looking at text in the extern bit (search area)
        op.write("template<>\n")
        if "_Expand" in search_area:
            op.write(
                "const char* FrameLib_ObjectName<FrameLib_Expand<"
                + fl_object_name
                + '>>::name()\n{ return "'
                + no_ext
                + '"; }\n'
            )
            op.write("\n")

        elif "_Expand" not in search_area and "makeClass" in search_area:
            op.write(
                "const char* FrameLib_ObjectName<FrameLib_Expand<"
                + fl_object_name
                + '>>::name()\n{ return "'
                + no_ext
                + '"; }\n'
            )
            op.write("\n")

        elif "_Expand" not in search_area and "makeClass" not in search_area:
            op.write(
                "const char* FrameLib_ObjectName<"
                + fl_object_name
                + '>::name()\n{ return "'
                + no_ext
                + '"; }\n'
            )
            op.write("\n")
