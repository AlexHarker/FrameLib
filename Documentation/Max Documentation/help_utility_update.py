import create_temp
import help_base
import help_merge
import help_reusable_tabs
import help_size_name
import cleanup
from framelib.utils import sign_off, space, hyp
from framelib.classes import Documentation


def main():
    docs = Documentation()

    create_temp.main(docs, False)

    # Creates the templates for each help file.
    # This is an outer shell containing generic information and framework to be filled in
    print("1. Creating help file templates")
    help_base.main(docs)
    hyp()

    print("2. Merging master templates with internal patchers")
    help_merge.main(docs)
    hyp()

    # Merges the common reusable tabs with the templates
    # This completes all patch contents
    print("3. Adding reusable tabs")
    help_reusable_tabs.main(docs)
    hyp()

    # Resizes and (internally) renames help files
    # This creates the finished help file
    print("4. Cosmetic changes")
    help_size_name.main(docs)
    hyp()

    cleanup.main(docs)
    print("Performing cleanup")

    print("Completed all python scripts.")


if __name__ == "__main__":
    main()
