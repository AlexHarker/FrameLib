import os, string
from shutil import copyfile

# Declare Source, Destination, Other vars #
source = "/Users/jamesbradbury/Desktop/fl.plus.c"
destination = "/Users/jamesbradbury/Desktop/c_files/"
base = "fl.plus"

# Open text file with object names and make copies of source file with those as names #
with open ("/Users/jamesbradbury/Desktop/objectnames.txt", "r") as list:
    for line in list:
        line = line.strip()
        cFile = destination+line+".c"
        copyfile(source, cFile)

# String replacement #
cdir = os.listdir(destination)
for i in cdir:
    replace = i[:-2]
    s = open(destination+i).read()
    s = s.replace("fl.plus", replace)
    f = open(destination+i, "w")
    f.write(s)
    f.close()

    



