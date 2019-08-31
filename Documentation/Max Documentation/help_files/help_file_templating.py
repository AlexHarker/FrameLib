import os
import sys
sys.path.append('../')
from shutil import copyfile
from FrameLibDocs.strippers import strip_extension
from FrameLibDocs.utils import remove_ds, cd_up, check_make



this_script = os.path.dirname(os.path.realpath(__file__))
help_file_folder = os.path.join(this_script, "templates")


def make_help_file(template, object_name):
    copyfile(
        template,
        os.path.join(help_file_folder, f"{object_name}.maxhelp"),
    )


check_make(help_file_folder)


root = cd_up(this_script, 3)
externals = os.path.join(root, "Current Test Version", "FrameLib", "externals")

master_template = os.path.join(this_script, "help_template.maxhelp")

# Create Help Files
print("Creating templated help files. \n")
for framelib_obj in remove_ds(os.listdir(externals)):
    framelib_obj = strip_extension(framelib_obj, 1)
    make_help_file(master_template, framelib_obj)
