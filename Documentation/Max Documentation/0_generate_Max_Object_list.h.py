from FrameLibDocs.classes import Documentation
from pathlib import Path
import sys

docs = Documentation()
op = open(docs.max_docs_dir / "Max_Object_List.h", "w+")



def write_comma(counter: int, ceiling: int) -> None:
    if counter < ceiling - 1:
        op.write(",")
        op.write("\n")
    else:
        op.write("\n")


def name_sanitisation(name: str) -> str:
    name = name.split(">")[0]
    name = name.split(",")[0]
    if name == "FrameLib_MaxClass_ToMax":
        name = "FrameLib_ToHost"
    elif name == "FrameLib_MaxClass_FromMax":
        name = "FrameLib_FromHost"
    elif name == "FrameLib_MaxClass_Info":
        name = "FrameLib_Info"
    elif name == "FrameLib_MaxClass_Read":
        name = "FrameLib_Read"
    elif name == "FrameLib_MaxClass_ComplexExpression":
        name = "FrameLib_ComplexExpression"
    elif name == "FrameLib_MaxClass_Expression":
        name = "FrameLib_Expression"
    elif name == "FrameLib_MaxClass_ContextControl":
        name = "FrameLib_ContextControl"
    return name


ignored_objects = [x for x in sys.argv[1:]]

def main(docs: Documentation):
    # Create the Max_Object_list.h and add skeleton
    op.write('#include "FrameLib_TypeList.h"\n\n')
    op.write("using FrameLib_DSPList = detail::FrameLib_Typelist<\n\n")

    # Directory formation
    max_objects = docs.repo_root / "FrameLib_Max_Objects"
    max_objects_categories = [x for x in max_objects.iterdir() if x.is_dir()]

    source_file_list = []
    ## Get folders in the parent Max Objects Folder
    for category in max_objects_categories:
        for name in Path(category).rglob("fl.*.cpp"):
            if name.stem not in ignored_objects:
                source_file_list.append([category, name])

    # TODO - lookwithin and reaise this is not okay
    # Recreate full paths to open and parse for type cases
    for counter, (category_folder, name) in enumerate(source_file_list):
        with open((Path(category_folder) / name), "r") as cpp:

            # flatten it with no spaces whatsoever
            source_file = cpp.read().replace("\n", "").replace(" ", "")
            search_area = source_file.split('extern"C"intC74_EXPORTmain(void){')[1]
            fl_object_name = name_sanitisation(search_area.split("<")[1])
            search_area = search_area.split("<")[0]

            # infer type with brutal checking by looking at text in the extern bit (search area)
            if "_Expand" in search_area:
                op.write(f"FrameLib_Expand<{fl_object_name}>")
                ## Condition to check if its the last one
            elif "_Expand" not in search_area and "makeClass" in search_area:
                op.write(f"FrameLib_Expand<{fl_object_name}>")
                ## Condition to check if its the last one
            elif "_Expand" not in search_area and "makeClass" not in search_area:
                op.write(fl_object_name)
            write_comma(counter, len(source_file_list))

    ## Demarcate end of this section
    op.write("\n\n>;\n\n")

    ## Start const bit
    for category_folder, name in source_file_list:
        with open(Path(category_folder) / name, "r") as cpp:

            source_file = cpp.read().replace("\n", "").replace(" ", "")  # flatten it with no spaces whatsoever
            search_area = source_file.split('extern"C"intC74_EXPORTmain(void){')[1]

            fl_object_name = name_sanitisation(search_area.split("<")[1])

            search_area = search_area.split("<")[0]
            # infer type with brutal checking by looking at text in the extern bit (search area)
            op.write("template<>\n")
            if "_Expand" in search_area:
                op.write(
                    "const char* FrameLib_ObjectName<FrameLib_Expand<"
                    + fl_object_name
                    + '>>::name()\n{ return "'
                    + name.stem
                    + '"; }\n'
                )

            elif "_Expand" not in search_area and "makeClass" in search_area:
                op.write(
                    "const char* FrameLib_ObjectName<FrameLib_Expand<"
                    + fl_object_name
                    + '>>::name()\n{ return "'
                    + name.stem
                    + '"; }\n'
                )

            elif "_Expand" not in search_area and "makeClass" not in search_area:
                op.write(
                    "const char* FrameLib_ObjectName<"
                    + fl_object_name
                    + '>::name()\n{ return "'
                    + name.stem
                    + '"; }\n'
                )
            op.write("\n")
    op.close()

if __name__ == "__main__":
    main(Documentation())
