import os
import shutil
from FrameLibDocs.utils import cd_up, get_path

root = cd_up(get_path(), 1)
tmp = os.path.join(root, "__tmp__")
shutil.rmtree(tmp)
