
from project_tools.vs_util import fl_solution
from project_tools.xc_util import fl_pbxproj
from project_tools.path_util import fl_paths
from project_tools.object_info import fl_object
from project_tools import file_util
from project_tools import code_util

from pathlib import Path

import argparse
import time
    

def time_result(name : str, t1, t2 = None):
    
    if t2 is None:
        t2 = time.perf_counter_ns()
        
    print("Completed " + name + " in " + str((t2 - t1)/1e9) + " seconds")


def update_all(add: bool):

    projects = Path(fl_paths().vs_max_projects()).glob('fl.*.vcxproj')
    project_list = list(projects)
    project_list.sort()
        

    # Loop over all max object VS projects
    
    for project in project_list:
        name = project.as_posix().rsplit("/", 1)[1].replace(".vcxproj", "")
        
        if add:
            print("add " + name)
        else:
            print("remove " + name)
        
        info = fl_object.create_from_name(name)

        # Overwrite the max object VS project to ensure it is up to date if we are adding
        
        if add:
            fl_solution().update_project(info, True)
        
        # Update the VS solution and Xcode project
        
        fl_solution().update(info, add)
        fl_pbxproj().update(info, add)


def add_all():
    
    # Update to add all projects with time reporting
    
    t1 = time.perf_counter_ns()
    update_all(True)
    time_result("removal", t1)


def remove_all():

    # Update to remove all projects with time reporting

    t1 = time.perf_counter_ns()
    update_all(False)
    time_result("removal", t1)
    

def rebuild():
    
    # Update to remove and then add all projects with time reporting

    t1 = time.perf_counter_ns()
    update_all(False)
    t2 = time.perf_counter_ns()
    update_all(True)
    
    time_result("removal", t1, t2)
    time_result("additions", t2)
    time_result("rebuild", t1)
    

def new_object(info : fl_object):
    
    paths = fl_paths()

    # Ensure that the category folders exist for the framelib object
    
    Path(paths.object_dir(info)).mkdir(parents = True, exist_ok = True)
    Path(paths.max_object_dir(info)).mkdir(parents = True, exist_ok = True)
    
    # Create a max oobject source file and source + header for the framelib object
    
    file_util.create(paths.max_source(info), paths.code_template("fl.class_name~.cpp"), info)
    file_util.create(paths.object_header(info), paths.code_template("FrameLib_Class.h"), info)
    file_util.create(paths.object_source(info), paths.code_template("FrameLib_Class.cpp"), info)
    
    # Update the single object build source files
    
    code_util.update_code(info)
    
    # Update the VS solution / create a max object VS project and update the Xcode project
    
    fl_solution().update_project(info)
    fl_solution().update(info, True)
    fl_pbxproj().update(info, True)
    
    # Rebuild the VS solution / max object VS project / Xcode project
    
    rebuild()
    
    
def main():

    # Argument parsing
    
    parser = argparse.ArgumentParser(description="Manipulate and update framelib projects")
    parser.add_argument("-o", "--object", default="FrameLib_Test", help="The framelib class")
    parser.add_argument("-n", "--name", help="The max object name")
    parser.add_argument("-c", "--category", default="Schedulers", help="The object category")
    parser.add_argument("-a", "--action", default="new", help="Sets the action (new / remove / add / rebuild")
    parser.add_argument("--cache", default=True, help="Read from the object cache (if present)")
    args = parser.parse_args()
    
    # Reload the cache unless specified not to
    
    if args.cache:
        fl_object.load_cache()
    
    # Derive the max object name from the framelib class name if it isn't explictly specified
    
    if args.name is None:
        args.name = "fl." + file_util.regex_search(args.object, ".*?([^_]*)$").lower() + "~"
    
    # Call the appropriate method based on the specified action
    
    if args.action == "new":
        new_object(fl_object(args.object, args.name, args.category))
    elif args.action == "remove":
        remove_all()
    elif args.action == "add":
        add_all()
    elif args.action == "rebuild":
        rebuild()

    # Sort the max object VS project contents
    
    fl_solution().sort_project()
    
    # Save the cache
    
    fl_object.save_cache()
    
    
if __name__ == "__main__":
    main()
