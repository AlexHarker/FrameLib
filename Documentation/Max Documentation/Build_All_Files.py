import parse_to_dlookup
import parse_to_qlookup
import parse_to_tlookup
import create_category_database
import create_tutorial_coll
import edit_raw_XML

def main():
    
    print('*' * 40)
    print('Building Category Database')
    create_category_database.main()
    print(' ' * 40)

    print('*' * 40)
    print('Editing XML Files')
    edit_raw_XML.main()
    print(' ' * 40)

    print('*' * 40)
    print('Building dlookup')
    parse_to_dlookup.main()
    print(' ' * 40)

    print('*' * 40)
    print('Building qlookup')
    parse_to_qlookup.main()
    print(' ' * 40)

    print('*' * 40)
    print('Building tlookup')
    parse_to_tlookup.main()
    print(' ' * 40)

    print('*' * 40)
    print('Building tutorial name coll')
    create_tutorial_coll.main()
    print(' ' * 40)

    print(' ' * 40)
    print("_-" * 40)
    print(' ' + "Completed all python files")
    print("-_" * 40)
    print(' ' * 40)

if __name__ == '__main__':
    main()


