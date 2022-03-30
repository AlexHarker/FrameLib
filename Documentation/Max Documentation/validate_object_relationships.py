
def main(docs):
    """
    Validate the entries in object relationships.
    Makes sure that all objects and seealsos exist.
    """

    object_names = [x.stem for x in docs.source_files]
    valid_names = object_names[:]
    
    valid_names.append("buffer~");
    
    for object in object_names:
        try:
            details = docs.object_info[object]
        except KeyError:
            raise KeyError(f"No entry for {object} in object_relationships.json")
            
        try:
            seealso = details["seealso"]
        except KeyError:
            raise KeyError(f"No seealso for {object} in object_relationships.json")
        
        for x in seealso:
            if x not in valid_names:
                raise ValueError(f"object {x} doesn't exist")
            

if __name__ == "__main__":
    main(Documentation())
