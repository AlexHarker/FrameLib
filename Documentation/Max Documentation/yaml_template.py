import os
from helpers import cd_up
from strippers import strip_space, strip_extension

def get_path():
    '''
    returns path of script being run
    '''
    return os.path.dirname(os.path.realpath(__file__))

root = get_path()

package = cd_up(root, 2)
max_objects = os.path.join(package, 'FrameLib_Max_Objects')

f = open(os.path.join(root, 'object_relationships.yaml'), 'w+')
all_objects = os.walk(max_objects)
for root, dirs, files in all_objects:
    for name in files:
        name = os.path.splitext(name)[0]
        if name[:2] == 'fl':
            f.write(f'{name}: \n')
            f.write(f'    keywords: \n')
            f.write(f'    seealso: \n')
            f.write('\n')
            

f.close()



