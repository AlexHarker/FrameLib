import os
import yaml
import json
from helpers import cd_up, remove_ds, get_path, write_json
from strippers import strip_space, strip_extension

script_root = get_path()

package = cd_up(script_root, 2)
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

master_dict = {} # What will essentially become the YAML file by saving a dict as yaml
inner_dict = {'keywords' : [], 'seealso' : []}
all_objects = os.walk(max_objects)
for root, dirs, files in all_objects:
    for name, category in zip(files, root):
        obj_name = os.path.splitext(name)[0]
        category = os.path.basename(root)
        if name[:2] == 'fl':
            inner_dict = {'keywords': [], 'seealso': []}
            other_files = os.listdir(root)
            other_files.remove(name)
            if '.DS_Store' in other_files:
                other_files.remove('.DS_Store')
            inner_dict['keywords'] = prefilled_info[category]
            inner_dict['seealso'] = [os.path.splitext(x)[0] for x in other_files]
            obj_dict = {
                obj_name : inner_dict
            }
            master_dict.update(obj_dict)

# Now write the YAML file
yaml_file_dir = os.path.join(script_root, 'object_relationships.yaml')

# We have to make an instance of a safe dumper and tell it where to ignore alias
noalias_dumper = yaml.dumper.SafeDumper
noalias_dumper.ignore_aliases = lambda self, master_dict: True

with open(yaml_file_dir, 'w+') as outfile:
    yaml.dump(master_dict, outfile, default_flow_style=False, Dumper=noalias_dumper)
    
