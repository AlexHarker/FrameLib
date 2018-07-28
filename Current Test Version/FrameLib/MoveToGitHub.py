from distutils.dir_util import copy_tree, remove_tree

gitExternals = '/Users/jamesbradbury/FrameLib-fork/build/framelib~/Build/Products/Public Testing/'
gitPackage = '/Users/jamesbradbury/FrameLib-fork/Current Test Version/FrameLib'
localExternals = '/Users/jamesbradbury/Documents/Max 7/Packages/FrameLib/externals/'
localPackage = '/Users/jamesbradbury/Documents/Max 7/Packages/FrameLib/'

# copies externals from Framelib-fork to local package
def copyBuild():
    copy_tree(gitExternals, localExternals)
    print("Copied externals from Fork to Max Package")

# copy the package from local max folder to repo
def copyPackage():
    remove_tree(gitPackage) # delete the destination folder so redundant files are removed
    copy_tree(localPackage, gitPackage)
    print("Copied package from Max Packages to Fork")
        
copyBuild()
copyPackage()