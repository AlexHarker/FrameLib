import os
from shutil import copyfile
from FrameLibDocs.strippers import strip_extension
from FrameLibDocs.utils import remove_ds, cd_up

this_script = os.getcwd()
help_file_folder = os.path.join(this_script, "help_file_templates")

# Check if the help_file_templates folder exists if not mk
def check_make(path_to_check):
    if not os.path.isdir(path_to_check):
        os.mkdir(path_to_check)
        print(f"Creating {path_to_check}.\n ")
    else:
        print(f"{path_to_check} already exists. \n")


def make_help_file(template, object_name):
    copyfile(
        template,
        os.path.join(this_script, "help_file_templates", f"{object_name}.maxhelp"),
    )


check_make(help_file_folder)


root = cd_up(this_script, 2)
externals = os.path.join(root, "Current Test Version", "FrameLib", "externals")

template = os.path.join(this_script, "fl.template.maxhelp")

# Create Help Files
print("Creating templated help files. \n")
for obj in remove_ds(os.listdir(externals)):
    obj = strip_extension(obj, 1)
    make_help_file(template, obj)
