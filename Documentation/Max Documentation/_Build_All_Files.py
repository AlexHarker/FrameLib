create_category_database = __import__("1_create_category_database")
edit_raw_XML = __import__("2_edit_raw_XML")
parse_to_dlookup = __import__("4_parse_to_dlookup")
parse_to_qlookup = __import__("5_parse_to_qlookup")
parse_to_tlookup = __import__("6_parse_to_tlookup")
parse_to_jlookup = __import__("7_parse_to_jlookup")
create_tutorial_coll = __import__("8_create_tutorial_coll")
cleanup = __import__("9_cleanup")
template_help = __import__("10_template_help")
merge_help = __import__("12_merge_help")

import os
from FrameLibDocs.utils import cd_up, get_path, sign_off, space, hyp
from FrameLibDocs.variables import max_docs_dir


def main():
    sign_off()
    space()

    # Stage 0
    # There is a prior stage here where make_object_list.py is called by Xcode.
    # This produces the header file which Build_Max_Docs.cpp uses to know about FrameLib objects and types.
    # Also, this where a number of temporary directories are created

    # Creates a category database in .json format.
    # The JSON file is used by edit_raw_XML.py to assign object categories to the xml files.
    print("1. Building Category Database")
    create_category_database.main()
    hyp()

    # The purpose of this script is to set the categories for the Raw_XML files.
    # C++ doesnt know about the categories at XML creation and its easier to iterate file structures in python.
    # Edited XML files are copied from /tmp/ to the refpages directory
    print("2. Editing XML Files")
    edit_raw_XML.main()
    hyp()

    # This script creates a dictionary used to display specific object info in the extras Max Patch.
    # Similar to the qlookup, but is specifically used to display the digest with mouse hovering
    print("4. Building dlookup")
    parse_to_dlookup.main()
    hyp()

    ## This script creates a dictionary that contains specific object information.
    # This provides the dynamic hover behaviour
    print("5. Building qlookup")
    parse_to_qlookup.main()
    hyp()

    # Creates a dictionary used to display names and descriptions of tutorials in the extras Max Patch.
    # The tutorials are categorised by difficulty. {Beginner, Intermediate, Advanced}
    print("6. Building tlookup")
    parse_to_tlookup.main()
    hyp()

    # Creates a dict containing information about object parameters. This is used by the help file template.
    print("7. Building jlookup")
    parse_to_jlookup.main()
    hyp()

    # Creates a coll containing the file names of the tutorials. Makes it a bit easier to load them.
    print("8. Building tutorial name coll")
    create_tutorial_coll.main()
    hyp()

    # # Deletes all temporay files and cleans up process
    print("9. Cleaning up")
    cleanup.main()
    hyp()

    # Creates the templates for each help file.
    # This is an outer shell containing generic information and framework to be filled in
    print("10. Creating help file templates")
    template_help.main()
    hyp()

    # Merges the hard coded tabs with the templates
    # This creates the finished help file
    print("12. Merging master templates with internal patchers")
    merge_help.main()
    hyp()

    print("Completed all python scripts.")

if __name__ == "__main__":
    main()
