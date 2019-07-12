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

prefilled_info = {

    'Binary' : ['binary ops, operators'],
    'Buffer' : ['buffer'],
    'Complex_Binary' : ['complex numbers', 'complex operators'],
    'Complex_Unary' : ['complex numbers', 'complex operators'],
    'Expressions' : ['expression'],
    'Filters' : ['filters'],
    'Generators' : ['generators'],
    'Host_Communication' : ['conversion'],
    'IO' : ['conversion'],
    'Mapping' : ['scaling', 'mapping'],
    'Parameters' : ['parameters'],
    'Routing' : ['routing'],
    'Schedulers' : ['timing, schedulers', 'triggers'],
    'Spatial' : ['spatialiastion'],
    'Spectral' : ['FFT, windowing, frequency domain'],
    'Storage' : ['storage'],
    'Streaming' : ['multistreaming', 'streams'],
    'Ternary' : ['ternary ops', 'operators'],
    'Time_Smoothing' : ['time smoothing', 'statistics'],
    'Timing' : ['timing'],
    'Unary' : ['unary ops', 'operators'],
    'Vector' : ['vector']
}

f = open(os.path.join(root, 'object_relationships.txt'), 'w+')
all_objects = os.walk(max_objects)
for root, dirs, files in all_objects:
    for name, category in zip(files, root):
        obj_name = os.path.splitext(name)[0]
        category = os.path.basename(root)
        if name[:2] == 'fl':
            other_files = os.listdir(root)
            other_files.remove(name)
            f.write(f'{obj_name}: \n')

            f.write(f'    keywords: \n')
            for key in prefilled_info[category]:
                f.write(f'        - {key} \n')
            
            f.write('\n')

            f.write(f'    seealso: \n')
            for other_file in other_files:
                other_file = os.path.splitext(other_file)[0]
                f.write(f'        - {other_file} \n')
            
            f.write('\n')
            
f.close()



