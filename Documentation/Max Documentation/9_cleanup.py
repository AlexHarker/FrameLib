import os
import shutil
from FrameLibDocs.variables import temporary_dir


def main():
    shutil.rmtree(temporary_dir)
