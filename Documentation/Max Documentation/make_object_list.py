import os
from helpers import get_path
from strippers import *
import re

## Important stuff
type_cases = [ ## cases that you can infer the type from
    'FrameLib_MaxClass<T>::makeClass',
    'FrameLib_MaxClass_Expand<T>',
    'FrameLib_MaxClass_Expand<T>::makeClass'
] 
root = get_path()

## Create the Max_Object_list.h and add skeleton
op = open(os.path.join(root, 'Max_Object_list_test.h'), 'w+')
op.write('#include "FrameLib_TypeList.h"')
op.write('\n \n')
op.write('using FrameLib_DSPList = FrameLib_Typelist<')
op.write('\n \n')

## Directory formation
max_source_folder = root.replace('/Documentation/Max Documentation', '/FrameLib_Max_Objects/') ## Folder containing folders of categories
max_source_categories = os.listdir(max_source_folder) ## A list of the categories. Is used to find all the source files.

## Try removing unnecessary stuff, otherwise throw some info that it was not there
try:
    max_source_categories.remove('_MaxSDK_')
except ValueError:
    print('No _MaxSDK_ to delete')
    pass

try:
    max_source_categories.remove('.DS_Store')
except ValueError:
    print('No .DS_Store')
    pass

try:    
    max_source_categories.remove('Common')
except ValueError:
    print('No common folder')
    pass

max_source_categories.sort() ## Sort it so that its in an order

## Get folders in the parent Max Objects Folder
## get total amount of files
for folder in max_source_categories:
for folder in max_source_categories:

    ## Make var containing the name of the folder for a given category
    category_folder = os.path.join(max_source_folder, folder)
    ## Make a list containing all of the source files in a single category
    file_list = os.listdir(category_folder)
    ## Get rid of ibuffer file when traversing to buffer ateogry
    if 'ibuffer' in file_list:
        file_list.remove('ibuffer')

    ## Recreate full paths to open and parse for type cases

    for f in file_list:
        with open(os.path.join(category_folder, f), 'r' ) as cpp:
            source_file = cpp.read().replace('\n', '').replace(' ', '') #flatten it with no spaces whatsoever
            search_area = source_file.split('extern"C"intC74_EXPORTmain(void){')[1]
            
            ## Do this just before info gets cleaved
            fl_object_name = search_area.split('<')[1]
            fl_object_name = fl_object_name.split('>')[0]
            ## if it contains ,kAllInputs remove
            if ',' in fl_object_name:
                fl_object_name = fl_object_name.split(',')[0]
            ## Now limit search area
            search_area = search_area.split('<')[0]

            # infer type with brutal checking by looking at text in the extern bit (search area)
            if '_Expand' in search_area:
                op.write(f'FrameLib_Expand<{fl_object_name}>')
                ## Condition to check if its the last one
                op.write('\n')
            elif '_Expand' not in search_area and 'makeClass' in search_area:
                print(fl_object_name, '2')
            elif '_Expand' not in search_area and 'makeClass' not in search_area:
                print(fl_object_name, '3')
    ## Demarcate end of this section
 

    ## add const stuff at end of file
    # for f in file_list:





## Get the case
## Format 


