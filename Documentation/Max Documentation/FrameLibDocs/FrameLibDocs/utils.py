import os
import random as rn
import json
import yaml


def strip_space(tostrip: str) -> str:
    """Strips whitespace from the start and end of a string"""
    tostrip = tostrip.lstrip()
    tostrip = tostrip.rstrip()
    return tostrip


def strip_extension(input_string: str, max_splits: int) -> str:
    """Strip the extension from a string, returning the file name"""
    output = input_string.rsplit(".", max_splits)[0]
    return output


def check_make(folder_check: str):
    """Takes a directory name, checks if it exists and makes."""
    if not os.path.isdir(folder_check):
        os.mkdir(folder_check)


def remove_ds(list_in: list) -> list:
    if ".DS_Store" in list_in:
        list_in.remove(".DS_Store")
    return list_in


def thin_list(list_to_thin: list, things_to_thin: list) -> list:
    """
    Takes a list_to_thin and removes each element of things_to_thin from it (if it exists)
    """
    for badness in things_to_thin:
        if badness in list_to_thin:
            list_to_thin.remove(badness)
    return list_to_thin


def write_json(json_file_path: str, dict_to_write: dict):
    """Takes a dictionary and writes it to JSON file."""
    path = os.path.dirname(json_file_path)
    check_make(path)
    with open(json_file_path, "w+") as fp:
        json.dump(dict_to_write, fp, indent=4)


def read_json(json_file_path: str) -> dict:
    """Takes a JSON file and returns a dictionary"""
    with open(json_file_path, "r") as fp:
        data = json.load(fp)
        return data


def read_yaml(yaml_file_path: str) -> dict:
    with open(yaml_file_path, "r") as stream:
        try:
            return yaml.safe_load(stream)
        except yaml.YAMLError as exc:
            print(exc)


def cd_up(path: str, num: int) -> str:
    """Given path, traverse num directories up from it"""
    t_path = path
    for _ in range(num):
        t_path = os.path.dirname(t_path)
    return t_path


def get_path():
    return os.path.dirname(os.path.realpath(__file__))

#-- Functions to make the console prints much more pleasant --#

def ast():
    """Print some asterisks"""
    print("*" * 20)


def hyp():
    """Print some hyphens"""
    print("-" * 20)


def uds():
    """Print some underscores"""
    print("_" * 20)


def space():
    """Makes some space!"""
    print(" " * 20)
    print(" " * 20)


def orsym():
    """Prints a |"""
    print("|" * 20)


def sign_off():
    """Sign off the docs ;)"""
    print(rn.choice(printouts))
    print(" ")
    print(
        "A DSP framework for arbitrary size frame processing with arbitrary sub-sample accurate timing."
    )
    print("Alex Harker, ajharker@gmail.com")
    print(
        "Max Documentation and Tutorials by James Bradbury, jamesbradbury93@gmail.com"
    )


printouts = [
    """                                                                              
                                                           .---.             
                          __  __   ___        __.....__    |   .--/|         
      _.._               |  |/  `.'   `.  .-''         '.  |   |__||         
    .' .._.-,.--.        |   .-.  .-.   '/     .-''"'-.  `.|   .--||         
    | '   |  .-. |   __  |  |  |  |  |  /     /________\   |   |  ||  __     
  __| |__ | |  | |.:--.'.|  |  |  |  |  |                  |   |  ||/'__ '.  
 |__   __|| |  | / |   \ |  |  |  |  |  \    .-------------|   |  |:/`  '. ' 
    | |   | |  '-`" __ | |  |  |  |  |  |\    '-.____...---|   |  ||     | | 
    | |   | |     .'.''| |__|  |__|  |__| `.             .'|   |__||\    / ' 
    | |   | |    / /   | |_                 `''-...... -'  '---'  |/\'..' /  
    | |   |_|    \ \._,\ '/                                       '  `'-'`   
    |_|           `--'  `"                                                   
""",
    """
   ______                        _      _ _     
  |  ____|                      | |    (_| |    
  | |__ _ __ __ _ _ __ ___   ___| |     _| |__  
  |  __| '__/ _` | '_ ` _ \ / _ | |    | | '_ \ 
  | |  | | | (_| | | | | | |  __| |____| | |_) |
  |_|  |_|  \__,_|_| |_| |_|\___|______|_|_.__/ 
""",
    """
.#######.########....###...##.....#.#######.##......###.########.
.##......##.....#...##.##..###...##.##......##.......##.##.....##
.##......##.....#..##...##.####.###.##......##.......##.##.....##
.######..########.##.....#.##.###.#.######..##.......##.########.
.##......##...##..########.##.....#.##......##.......##.##.....##
.##......##....##.##.....#.##.....#.##......##.......##.##.....##
.##......##.....#.##.....#.##.....#.#######.#######.###.########.
""",
    """                                                                                 
  _|_|_|_|                                                _|        _|  _|        
  _|        _|  _|_|    _|_|_|  _|_|_|  _|_|      _|_|    _|            _|_|_|    
  _|_|_|    _|_|      _|    _|  _|    _|    _|  _|_|_|_|  _|        _|  _|    _|  
  _|        _|        _|    _|  _|    _|    _|  _|        _|        _|  _|    _|  
  _|        _|          _|_|_|  _|    _|    _|    _|_|_|  _|_|_|_|  _|  _|_|_|                                                                                                                                                              
""",
    """
 `7MM""YMM                                       `7MMF'        db *MM        
   MM    `7                                         MM              MM        
   MM   d `7Mb,od8 ,6"Yb. `7MMpMMMb.pMMMb.  .gP"Ya  MM        `7MM  MM,dMMb.  
   MM""MM   MM' "'8)   MM   MM    MM    MM ,M   Yb MM          MM  MM    `Mb 
   MM   Y   MM     ,pm9MM   MM    MM    MM 8M"""
    """ MM      ,   MM  MM     M8 
   MM       MM    8M   MM   MM    MM    MM YM.    , MM     ,M   MM  MM.   ,M9 
 .JMML.   .JMML.  `Moo9^Yo.JMML  JMML  JMML.`Mbmmd.JMMmmmmMMM .JMML.P^YbmdP'
""",
    """
 ___                            _       _     
(  _`\                         ( )    _( )    
| (_(__ __  _ _  ___ ___    __ | |   (_| |_   
|  _)( '__/'_` /' _ ` _ `\/'__`| |  _| | '_`\ 
| |  | | ( (_| | ( ) ( ) (  ___| |_( | | |_) )
(_)  (_) `\__,_(_) (_) (_`\____(____/(_(_,__/'
""",
]
