import argparse
import create_temp
import validate_object_relationships
import create_category_database
import edit_xml
import lookup_dlookup
import lookup_qlookup
import lookup_tlookup
import lookup_jlookup
import create_tutorial_coll
import cleanup
import help_base
import help_merge
import help_reusable_tabs
import create_max_db
from framelib.utils import sign_off, space, hyp
from framelib.classes import Documentation


def main():
    docs = Documentation()
    parser = argparse.ArgumentParser(description="Build Documentation for FrameLib")
    parser.add_argument("-hf", "--helpfiles", default=True, action='store_false', help="Toggle to switch off building help files")
    parser.add_argument("-c", "--clean", default=True, action='store_false', help="Toggle to switch off post-cleanup")
    parser.add_argument("-p", "--package", help="Provide a custom location for a package")
    args = parser.parse_args()

    if args.package:
        docs.set_package(args.package)

    create_temp.main(docs)

    sign_off()
    space()

    # Stage 0
    # There is a prior stage here where make_object_list.py is called by Xcode.
    # This produces the header file which Build_Max_Docs.cpp uses to know about FrameLib objects and types.
    # Also, this where a number of temporary directories are created

    # Validates that the seealso contents exist and that each object has an entry
    print("1. Validating object relationships")
    validate_object_relationships.main(docs)
    hyp()
    
    # Creates a category database in .json format.
    # The JSON file is used by 2_edit_raw_XML.py to assign object categories to the xml files.
    print("2. Building Category Database")
    create_category_database.main(docs)
    hyp()

    # The purpose of this script is to set the categories for the Raw XML files.
    # C++ doesnt know about the categories at XML creation.
    # Edited XML files are copied from /tmp/ to the refpages directory
    print("3. Editing XML Files")
    edit_xml.main(docs)
    hyp()

    # This script creates a dictionary used to display specific object info in the extras Max Patch.
    # Similar to the qlookup, but is specifically used to display the digest with mouse hovering
    print("4. Building dlookup")
    lookup_dlookup.main(docs)
    hyp()

    ## This script creates a dictionary that contains specific object information.
    # This provides the dynamic hover behaviour
    print("5. Building qlookup")
    lookup_qlookup.main(docs)
    hyp()

    # Creates a dictionary used to display names and descriptions of tutorials in the extras Max Patch.
    # The tutorials are categorised by difficulty. {Beginner, Intermediate, Advanced}
    print("6. Building tlookup")
    lookup_tlookup.main(docs)
    hyp()

    # Creates a dict containing information about object parameters. This is used by the help file template.
    print("7. Building jlookup")
    lookup_jlookup.main(docs)
    hyp()

    # Creates a coll containing the file names of the tutorials. Makes it a bit easier to load them.
    print("8. Building tutorial name coll")
    create_tutorial_coll.main(docs)
    hyp()

    if args.helpfiles:
        # Creates the templates for each help file.
        # This is an outer shell containing generic information and framework to be filled in
        print("10. Creating help file templates")
        help_base.main(docs)
        hyp()

        print("11. Merging master templates with internal patchers")
        help_merge.main(docs)
        hyp()

        # Merges the common reusable tabs with the templates
        # This creates the finished help file
        print("12. Adding reusable tabs")
        help_reusable_tabs.main(docs)
        hyp()

    # Creates a database of files to exclude
    print("10. Creating help file templates")
    create_max_db.main(docs)
    hyp()

    if args.clean:
        cleanup.main(docs)
        print("Performing cleanup")

    print("Completed all python scripts.")


if __name__ == "__main__":
    main()
