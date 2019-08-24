import os
import shutil
from FrameLibDocs.utils import cd_up, get_path

def main(root):
    tmp = os.path.join(root, "__tmp__")
    shutil.rmtree(tmp)
