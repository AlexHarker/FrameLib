import os, sys
from shutil import copyfile
from FrameLibDocs.utils import remove_ds, cd_up, check_make, read_json, write_json, strip_extension


this_script = os.path.dirname(os.path.realpath(__file__))
help_file_folder = os.path.join(this_script, "help_files", "templates")

check_make(help_file_folder)

root = cd_up(this_script, 2)
externals = os.path.join(root, "Current Test Version", "FrameLib", "externals")

master_template = os.path.join(this_script, "help_files", "help_template.maxhelp")

def make_help_file(template, output_name):
    copyfile(
        template,
        os.path.join(help_file_folder, f'{output_name}.maxhelp')
    )

def edit_help_file(file_edit, obj_name):
    t_json = read_json(file_edit)
    outer_boxes = t_json['patcher']['boxes'][0]
    internal_boxes = outer_boxes['box']['patcher']['boxes']
     
    for key in internal_boxes:
        if 'jsarguments' in key['box']:
            key['box']['jsarguments'] = obj_name

    write_json(file_edit, t_json)
    

def main():
    for framelib_obj in remove_ds(os.listdir(externals)):
        name = os.path.splitext(framelib_obj)[0]
        ext = os.path.splitext(framelib_obj)[1]
        if  ext == '.mxe64' or ext == '.mxo':
            framelib_obj = strip_extension(framelib_obj, 1)
            make_help_file(master_template, framelib_obj)
            edit_help_file(
                file_edit = os.path.join(help_file_folder, f'{name}.maxhelp'),
                obj_name  = name
            )