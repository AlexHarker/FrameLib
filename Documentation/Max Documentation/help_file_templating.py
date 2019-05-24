import helpers as hp
import os
from strippers import strip_extension
from shutil import copyfile

def make_help_file(template, object_name):
    copyfile(template, f'{root}/help_file_templates/{object_name}.maxhelp')


root = hp.get_path()
externals = root.replace('/Documentation/Max Documentation', '/Current Test Version/FrameLib/externals')
template = f'{root}/fl.template.maxhelp'
for obj in os.listdir(externals):
    if obj != '.DS_Store':
        obj = strip_extension(obj, 1)
        make_help_file(template, obj)