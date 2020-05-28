import os, sys, json
from glob import glob
from pathlib import Path
from FrameLibDocs.utils import read_json, write_json, strip_extension

binary = [] # binary files all have mismatch + trigger
ternary = [] # ternary files all have mismatch

mismatch = open(Path("help_files/mismatch_template.maxhelp"), "r").read()
trigger_ins = open(Path("help_files/trigger_ins_template.maxhelp"), "r").read()

def append_tabs(patch, object_name, source):
    """Take a string, replace FLOBJECTNAME and append the tabs to the source"""
    modified_patch = patch.replace("FLOBJECTNAME", object_name)
    converted = json.loads(modified_patch)
    for tab in converted['patcher']['boxes']:
        source['patcher']['boxes'].append(tab)

# Find binary and ternary objects
p = Path("../../FrameLib_Max_Objects")
for z in p.glob("**/*"):
    object_name = z.stem
    category = z.parent.stem
    if object_name[:3] == "fl.":
        if category == "Binary":
            binary.append(object_name)
        if category == "Ternary":
            ternary.append(object_name)

p = Path('help_files/templates')
for object_path in p.glob('**/*'):
    if object_path.stem in binary or object_path.stem in ternary:
        template = read_json(object_path)
        append_tabs(mismatch, object_path.stem, template)

        if object_path.stem in binary:
            append_tabs(trigger_ins, object_path.stem, template) 

        write_json(
            Path(object_path), 
            template
        )

# Now move all the help files to the project directory



    













