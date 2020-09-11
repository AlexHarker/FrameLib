from pathlib import Path

this_script = Path(__file__)
max_docs_dir = this_script.parents[2]
package_root = this_script.parents[4]

# Import Max Paths
current_version = package_root / "Current Test Version"

# Temporary Directories
temporary_dir = max_docs_dir / "__tmp__"
databases_dir = temporary_dir / "db"
raw_xml_dir = temporary_dir / "raw_xml"

# Key Files
category_database_path = databases_dir / "category_databases.json"
object_relationships_path = max_docs_dir / "object_relationships.yaml"

# Max specific paths
refpages_dir = current_version / "FrameLib" / "docs" / "refpages"
interfaces_dir =  current_version / "FrameLib" / "interfaces"

# Help Files
help_dir = max_docs_dir / "help_files"
