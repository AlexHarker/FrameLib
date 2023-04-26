
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
        
    print("Completed " + name + " in " + str((t2 - t1)/1000000000.) + " seconds")


def update_all(add: bool):

    projects = Path(fl_paths().vs_max_projects()).glob('fl.*.vcxproj')
    project_list = list(projects)
    project_list.sort()
        
    for project in project_list:
        name = project.as_posix().rsplit("/", 1)[1].replace(".vcxproj", "")
        
        if add:
            print("add " + name)
        else:
            print("remove " + name)
        
        object_info = fl_object.create_from_name(name)

        if add:
            fl_solution().update_project(object_info, True)
        
        fl_solution().update(object_info, add)
        fl_pbxproj().update(object_info, add)


def add_all():
    
    t1 = time.perf_counter_ns()
    update_all(True)
    time_result("removal", t1)


def remove_all():

    t1 = time.perf_counter_ns()
    update_all(False)
    time_result("removal", t1)
    

def rebuild():
    
    t1 = time.perf_counter_ns()
    update_all(False)
    t2 = time.perf_counter_ns()
    update_all(True)
    
    time_result("removal", t1, t2)
    time_result("additions", t2)
    time_result("rebuild", t1)
    

def new_object(object_info : fl_object):
    
    paths = fl_paths()

    Path(paths.object_dir(object_info)).mkdir(parents = True, exist_ok = True)
    Path(paths.max_object_dir(object_info)).mkdir(parents = True, exist_ok = True)
    
    file_util.create(paths.max_source(object_info), paths.code_template("fl.class_name~.cpp"), object_info)
    file_util.create(paths.object_header(object_info), paths.code_template("FrameLib_Class.h"), object_info)
    file_util.create(paths.object_source(object_info), paths.code_template("FrameLib_Class.cpp"), object_info)
    
    code_util.update_code(object_info)    
    fl_solution().update_project(object_info)
    fl_solution().update(object_info, True)
    fl_pbxproj().update(object_info, True)
    rebuild()
    
    
def main():

    parser = argparse.ArgumentParser(description="Manipulate and update framelib projects")
    parser.add_argument("-o", "--object", default="FrameLib_Test", help="The framelib class")
    parser.add_argument("-n", "--name", help="The max object name")
    parser.add_argument("-c", "--category", default="Schedulers", help="The object category")
    parser.add_argument("-a", "--action", default="new", help="Sets the action (new / remove / add / rebuild")
    parser.add_argument("--cache", default=True, help="Read from the object cache (if present)")
    args = parser.parse_args()
    
    if args.cache:
        fl_object.load_cache()
    
    if args.name is None:
        args.name = "fl." + file_util.regex_search(args.object, ".*?([^_]*)$").lower() + "~"
            
    if args.action == "new":
        new_object(fl_object(args.object, args.name, args.category))
    elif args.action == "remove":
        remove_all()
    elif args.action == "add":
        add_all()
    elif args.action == "rebuild":
        rebuild()

    fl_solution().sort_project()
    
    fl_object.save_cache()
    
    
if __name__ == "__main__":
    main()
