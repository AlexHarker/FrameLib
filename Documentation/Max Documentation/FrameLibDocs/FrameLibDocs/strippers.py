#### Some useful functions that strip various substrings from strings ####


def strip_space(tostrip):
    tostrip = tostrip.lstrip()
    tostrip = tostrip.rstrip()
    return tostrip


def strip_extension(input_string, maxsplits):
    output = input_string.rsplit(".", maxsplits)[0]
    return output
