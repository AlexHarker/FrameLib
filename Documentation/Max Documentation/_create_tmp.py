import os
from FrameLibDocs.utils import check_make
from FrameLibDocs.variables import (
    max_docs_dir,
    temporary_dir,
    databases_dir,
    raw_xml_dir,
)

# Check and make
check_make(temporary_dir)
check_make(databases_dir)
check_make(raw_xml_dir)
