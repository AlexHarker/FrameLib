from framelib.classes import Documentation
import xml.etree.ElementTree as et

def refpage_name(docs: Documentation, obj_name: str) -> str:
    """Returns the refpage name of an object"""
    file_name = obj_name + ".maxref.xml"
    ref_path = docs.refpages_dir / "framelib-ref" / file_name
    return et.parse(ref_path).getroot().get("name")
    
