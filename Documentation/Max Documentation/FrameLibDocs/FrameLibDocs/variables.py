import os
from FrameLibDocs.utils import cd_up

this_script = os.path.dirname(os.path.realpath(__file__))
max_docs_dir = cd_up(this_script, 2)
package_root = cd_up(this_script, 4)

# Temporary Directories
temporary_dir = os.path.join(max_docs_dir, "__tmp__")
databases_dir = os.path.join(temporary_dir, "db")
raw_xml_dir = os.path.join(temporary_dir, "raw_xml")

# Key Files
category_database_path = os.path.join(databases_dir, "category_database.json")
object_relationships_path = os.path.join(max_docs_dir, "object_relationships.yaml")

# Max specific paths
refpages_dir = os.path.join(
    package_root, "Current Test Version", "FrameLib", "docs", "refpages"
)
interfaces_dir = os.path.join(
    package_root, "Current Test version", "FrameLib", "interfaces"
)

# Common objectives/variables
bad_entries = [".DS_Store", "_c74_ref_modules.xml"]
