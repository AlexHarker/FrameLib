import os, sys, json
from distutils.dir_util import copy_tree
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

def main():
# Find binary and ternary objects
    p = Path("../../FrameLib_Max_Objects")
    for obj in p.glob("**/*"):
        object_name = obj.stem
        category = obj.parent.stem
        if object_name[:3] == "fl.":
            if category == "Binary":
                binary.append(object_name)
            if category == "Ternary":
                ternary.append(object_name)

    # Now insert the necessary tabs 
    templates = Path('help_files/templates')
    for object_path in templates.glob('**/*'):
        if object_path.stem in binary or object_path.stem in ternary:
            template = read_json(object_path)
            append_tabs(mismatch, object_path.stem, template)

            if object_path.stem in binary:
                append_tabs(trigger_ins, object_path.stem, template) 

            write_json(
                object_path,
                template
            )

    # Now collect up and move all the templates to the dist
    # We could do this in the previous loop, but I think is clearer
    dest = Path("../../Current Test Version/FrameLib/help")
    copy_tree(
        str(templates),
        str(dest),
        update=1
    )
    


