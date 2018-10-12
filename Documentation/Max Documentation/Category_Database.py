import os


dir_path = os.path.dirname(os.path.realpath(__file__))
dir_path = dir_path.replace('/Documentation/Max Documentation', '/FrameLib_Max_Objects')

print(os.walk(dir_path))