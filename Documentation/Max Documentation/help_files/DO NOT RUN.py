import os
import sys
from shutil import copyfile
sys.path.append('../')
from FrameLibDocs.strippers import strip_extension
from FrameLibDocs.utils import remove_ds, cd_up, check_make, read_json, write_json


this_script = os.path.dirname(os.path.realpath(__file__))
internal_folder = os.path.join(this_script, "internal_tabs")

root = cd_up(this_script, 3)
externals = os.path.join(root, "Current Test Version", "FrameLib", "externals")

master_template = os.path.join(this_script, "internal_template.maxhelp")


def make_help_file(template, output_name):
    copyfile(
        template,
        os.path.join(internal_folder, f'{output_name}.maxhelp')
    )


def edit_help_file(file_edit, obj_name):
    t_json = read_json(file_edit)
    outer_boxes = t_json['patcher']['boxes']
    for k in outer_boxes:
        for z in k['box']:
            if k['box'][z] == 'p basic':
                k['box']['patcher']['boxes'][1]['box']['jsarguments']=obj_name
    write_json(file_edit, t_json)            

if __name__ == '__main__':
    print("Creating templated help files.\n")
    for framelib_obj in remove_ds(os.listdir(externals)):
        ext = os.path.splitext(framelib_obj)[1]
        name = os.path.splitext(framelib_obj)[0]
        if ext == '.mxe64' or ext == '.mxo':
            framelib_obj = strip_extension(framelib_obj, 1)
            make_help_file(master_template, framelib_obj)
            edit_help_file(
                file_edit=os.path.join(internal_folder, f'{name}.maxhelp'),
                obj_name=name
                )