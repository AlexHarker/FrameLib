from framelib.utils import write_json
from framelib.classes import Documentation
import re

def list_files(path):
    return [ x.name for x in path.iterdir() if x.suffix != "" ]
    
def main(docs):
    """
    Create the max db file for exclusions from the package.
    """

    d_db = {};
    d_aliases = {}
    d_inner = {}
    
    filters = [ { "filter" : "ignore", "path" : "package@FrameLib:/misc", "kind" : "*" },
                { "filter" : "ignore", "path" : "package@FrameLib:/docs/tutorial-patchers", "kind" : "*" }]

    object_mapping_path = docs.package / "init" / "fl-objectmappings.txt"
    misc_path = docs.package / "misc"
    tut_patcher_path = docs.package / "docs" / "tutorial-patchers"
    db_path = docs.interfaces_dir / "max.db.json"
    
    f = open(object_mapping_path, "r")
    
    for line in f:
        m = re.match("max objectfile (.+) (.*);", line)
        d_aliases[m.group(1)] = m.group(2)
    
    f.close;
    
    d_inner["filters"] = filters
    d_inner["aliases"] = d_aliases
    d_inner["exclusions"] = list_files(tut_patcher_path) + list_files(misc_path)
    d_inner["exclusions"].sort()
    d_db["maxdb"] = d_inner
    
    write_json(db_path, d_db)

    
if __name__ == "__main__":
    main(Documentation())
