import os
from FrameLibDocs.utils import get_path, check_make, cd_up

root = cd_up(get_path(), 1)

# Directories
temporary = os.path.join(root, "__tmp__")
databases = os.path.join(temporary, "db")
raw_xml   = os.path.join(temporary, "raw_xml")

# Check and make
check_make(temporary)
check_make(databases)
check_make(raw_xml)
