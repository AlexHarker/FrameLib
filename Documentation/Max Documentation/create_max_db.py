from framelib.utils import write_json
from framelib.classes import Documentation

def list_files(path):
    return [ x.name for x in path.iterdir() if x.suffix != "" ]
    
def main(docs):
    """
    Create the max db file for exclusions from the package.
    """

    d_db = {};
    d_inner = {}
    
    tut_patcher_path = docs.package / "docs" / "tutorial-patchers"
    misc_path = docs.package / "misc"
    db_path = docs.interfaces_dir / "max.db.json"
    
    d_inner["exclusions"] = list_files(tut_patcher_path) + list_files(misc_path)
    d_inner["exclusions"].sort()
    d_db["maxdb"] = d_inner
    
    write_json(db_path, d_db)

    
if __name__ == "__main__":
    main(Documentation())
