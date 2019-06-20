import os
import random as rn
import json

def remove_ds(list_in):
    if '.DS_Store' in list_in:
        list_in.remove('.DS_Store')
        return list_in

def write_json(json_file, in_dict):
    '''
    Takes a dictionary and writes it to JSON file.

    Args:
        json_file: A path to where the JSON file will be written.
        in_dict: A dictionary that will be saved as JSON.
    Returns:
        None
    '''
    with open(json_file, 'w+') as fp:
        json.dump(in_dict, fp, indent=4)

def read_json(json_file):
    '''
    Takes a JSON file and returns a dictionary

    Args:
        json_file: A path to a JSON file that will be read.
    Returns:
        A python dictionary.
    '''
    with open(json_file, 'r') as fp:
        data = json.load(fp)
        return data

def cd_up(path, num):
    '''
    Given path, traverse num directories up from it
    '''
    t_path = path
    for _ in range(num):
        t_path = os.path.dirname(t_path)
    return t_path

def get_path():
    return os.path.dirname(os.path.realpath(__file__))

def ast():
    '''
    Print some asterisks
    '''
    print('*' * 20)

def hyp():
    '''
    Print some hyphens
    '''
    print('-' * 20)

def uds():
    '''
    Print some underscores
    '''
    print('_' * 20)

def space():
    '''
    Makes some space!
    '''
    print(' ' * 20)
    print(' ' * 20)

def orsym():
    '''
    Prints a |
    '''
    print('|' * 20)

def sign_off():
    '''
    Sign off the docs ;)
    '''
    print(rn.choice(printouts))
    print(' ')
    print('A DSP framework for arbitrary size frame processing with arbitrary sub-sample accurate timing.')
    print('Alex Harker, ajharker@gmail.com')
    print('Max Documentation and Tutorials by James Bradbury, jamesbradbury93@gmail.com')


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
   MM   Y   MM     ,pm9MM   MM    MM    MM 8M"""""" MM      ,   MM  MM     M8 
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
"""
]



                                                 
