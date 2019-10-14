import os
from shutil import rmtree
from FrameLibDocs.utils import check_make
from FrameLibDocs.variables import (
    max_docs_dir,
    temporary_dir,
    databases_dir,
    raw_xml_dir,
    interfaces_dir,
    refpages_dir
)

# Check and make
check_make(temporary_dir)
check_make(databases_dir)
check_make(raw_xml_dir)

# Clean out directories

# Interfaces
for files in os.listdir(interfaces_dir):
    try:
        os.remove(os.path.join(interfaces_dir, files))
    except OSError:
        print('Error cleaning out existing docs directories')
        pass

# Refpages
for files in os.listdir(refpages_dir):
    if os.path.isdir(os.path.join(refpages_dir, files)):
        try:
            rmtree(os.path.join(refpages_dir, files))
        except OSError:
            print('Error cleaning out existing docs directories')
            pass