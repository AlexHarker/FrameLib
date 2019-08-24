# FrameLib Documentation

## Welcome
You are able to build the documentation for FrameLib yourself, except for certain resources such as:

1. Max help files
2. Max tutorials

Some elements of the above resources are automatically generated though, like `.json` files and `.txt` files that are automatically loaded into dictionaries in colls. For example, the landing page of the FrameLib tutorials contains `umenu` objects which are automatically populated with all the objects that exist in the source code. This means that if you fork FrameLib and create your own objects using the Framework, the documentation builder can be run in order to automatically create a number of things for you.

## How to build

### Installing Xcode 9.4.1

You will need to download Xcode. Building has been tested on version 9.4.1. The most recent version as 23/08/19 is Version 10.3. To download an old version you will need to look beyond the app store and follow these tops.

1. Go to https://developer.apple.com/download/more/
2. Follow the prompts

Note you may need an account to get the older versions of xcode.

### Installing Python 3.6+

A large part of the project uses Python to generate and parse XML files as well as form `.json` and `.txt` references. MacOS version 10.14 and lower come preinstalled with Python 2 but this project is written using Python 3. As such, you will need to install a new version of Python. 

THe easiest way to do this is to use a package manager like `brew` which not only manages the install and updates but will create all the useful symlinks and aliases for the python executables and pip.

The website and guide on installing brew can be found here: 

https://brew.sh

To install brew copy this command into your shell: 

`/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"`

Once brew is installed you can install python by running the command: 

`brew install python3`

This will create the alias `python3` to your python 3 executable. This is essential as the Xcode project will ask your shell

If you don't want to install brew to manage your python install, or you already have it managed another way then you will need to make sure that python version 3 can be called by invoking the command `python3` from your terminal. As a last resort you can modify the build settings for the xcode project scheme 'Documentation'. You will want to modify the pre and post shell commands and the language that is used for calling on python.

### Building

Once you have a working version of Xcode 9.4.1 and Python 3.6+ there are a few options to build.

The easiest way is to open the `framelib~-documentation.xcodeproj` and build (Cmd + B) the 'Documentation' scheme. Alternatively you can execute the build from the command-line assuming you have the xcode command line tools installed. The command for doing so is:

`xcodebuild -project framelib~-documentation.xcodeproj -scheme Documentation build`

More information on building xcode projects on the command line can be found here:

https://developer.apple.com/library/archive/technotes/tn2339/_index.html


### Incorporating your own objects into the docs

There are a number of things you will have to do to incorporate your own objects into the docs although most of the work is automagically completed for you.

Once you have created your FrameLib_YOUROBJECT.cpp, FrameLib_YOUROBJECT.h and fl.YOUROBJECT~.cpp source code, the build process takes care of producing `.maxref.xml` files for you (adding it to the Max database) as well as adding it into the custom database for the FrameLib package. This means that your object will have autocompletion, help strings and will appear in the reference under the FrameLib package. It will also appear in the tutorials and extras.

You will need to make your own help files and tutorials for the object however, which is not covered in the automatic building of FrameLib documentation.

Prior to building the documentation you will also need to add an entry for your object in the `object_relationships.yaml` file. This provides the build process with knowledge of objects which are related to yours as well as any keywords that you want to associate with your object, and will populate the Max documentation with this information. Follow the format used for the native FrameLib objects. 


## An deeper explanation

The docs are built in a number of stages that involve creating instances of those objects and extracting the information that way. It is important to note that all information is stored in the source code itself as info strings. This means that you should make this information as concise and clear as possible. Try and follow the convention of objects that exist in the native FrameLib package. 

1. Execute Python to create Max_Object_List.h

This python script creates the Max_Object_List.h header file. This is essential for the next stage of the project which iterates over this file to create an instance of each object and to extract its information strings.

2. Extract information strings using Build_Max_Docs.cpp

This creates an instance of every FrameLib object and calls a method that will extract the output from each and format it into a .maxref.xml file.

3. Call "_Build_All_files.py" to execute the other python scripts. The specific functions of each of these is tersely documented in _Build_All_Files.py as well as each file itself.

Done!
