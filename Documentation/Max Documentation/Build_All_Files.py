import parse_to_dlookup
import parse_to_qlookup
import parse_to_tlookup
import create_category_database
import create_tutorial_coll
import edit_raw_XML
import helpers as hp

def main():
    root = hp.get_path()
    hp.sign_off()
    hp.space()
    
    print('1. Building Category Database')
    create_category_database.main(root)
    hp.hyp()

    print('2. Editing XML Files')
    edit_raw_XML.main(root)
    hp.hyp()

    print('3. Building dlookup')
    parse_to_dlookup.main(root)
    hp.hyp()

    print('4. Building qlookup')
    parse_to_qlookup.main(root)
    hp.hyp()

    print('5. Building tlookup')
    parse_to_tlookup.main(root)
    hp.hyp()

    print('6. Building tutorial name coll')
    create_tutorial_coll.main(root)
    hp.hyp()
    print(' ')
    print("Completed all python scripts!!!")


if __name__ == '__main__':
    main()


