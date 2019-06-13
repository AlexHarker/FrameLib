import os
from helpers import get_path

root = get_path()
object_List = f= open(f'{root}/Max_Object_list.h','w+')
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


