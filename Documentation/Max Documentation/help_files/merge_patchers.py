import os, sys, json
from FrameLibDocs.utils import read_json, write_json, strip_extension

# the path of this script
this_script = os.path.dirname(os.path.realpath(__file__))
templated_files = os.listdir(
    os.path.join(this_script, 'templates')
)
for i in templated_files:
    if i[:3] != 'fl.':
        templated_files.remove(i)

internal_tabs = os.listdir(
    os.path.join(this_script, 'internal_tabs')
)

for i in templated_files:
    template = read_json(os.path.join(this_script, 'templates', i))
    object_name = strip_extension(i, 1)
    companion_name = f'{object_name}.maxhelp'
    # now look for a patcher containing tabs
    if companion_name in internal_tabs:
        template_boxes = template['patcher']['boxes']
        tabs = read_json(os.path.join(this_script, 'internal_tabs', companion_name ))
        tabs_boxes = tabs['patcher']['boxes']
        for box in tabs_boxes:
            template['patcher']['boxes'].append(box)
        
        write_json(
            os.path.join(this_script, 'templates', f'{object_name}.maxhelp'),
            template, 
            )


    













