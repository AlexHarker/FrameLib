import a_create_category_database
import b_edit_raw_XML
import c_parse_to_dlookup
import d_parse_to_qlookup
import e_parse_to_tlookup
import f_parse_to_jlookup
import g_create_tutorial_coll
import helpers as hp

def main():
    root = hp.get_path()
    hp.sign_off()
    hp.space()
    
    print('1. Building Category Database')
    a_create_category_database.main(root)
    hp.hyp()

    print('2. Editing XML Files')
    b_edit_raw_XML.main(root)
    hp.hyp()

    print('3. Building dlookup')
    c_parse_to_dlookup.main(root)
    hp.hyp()

    print('4. Building qlookup')
    d_parse_to_qlookup.main(root)
    hp.hyp()

    print('5. Building tlookup')
    e_parse_to_tlookup.main(root)
    hp.hyp()

    print('6. Building jlookup')
    f_parse_to_jlookup.main(root)
    hp.hyp()

    print('7. Building tutorial name coll')
    g_create_tutorial_coll.main(root)
    hp.hyp()
    print(' ')
    print("Completed all python scripts!!!")


if __name__ == '__main__':
    main()


