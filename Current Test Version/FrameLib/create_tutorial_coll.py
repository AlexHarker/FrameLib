import os

def main():
    # Directory stuff #
    dir_path = os.path.dirname(os.path.realpath(__file__))
    tutorial_path = dir_path + '/docs/tutorial-patchers/'
    coll_output_path = f'{tutorial_path}tutorial_names.txt'
    print(coll_output_path)

    # If dir doesnt exist make, otherwise edit existing #
    coll = open(coll_output_path, 'w+')
    coll.truncate(0)

    # Find File Names #
    all_files = os.listdir(tutorial_path) #store all the file names of the tutorial path in an array

    # Only get the tutorial names
    tutorial_names = []

    # Have to do an additional loop here to sort all the tutorials alpha-numerically #
    for item in all_files:
        if item.startswith('_'):
            tutorial_names.append(item)
    tutorial_names.sort()

    # Write the contents of tutorial_names to the coll with some f strings #
    idx = 0
    for item in tutorial_names: #each item loop over all tutorial names list
        coll.write(f'{idx}, {item};\n') #write that item into the buffer
        idx += 1

    coll.close()






