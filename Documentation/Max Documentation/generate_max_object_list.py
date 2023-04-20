from framelib.classes import Documentation
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
    lookup = {
        "FrameLib_MaxClass_ToMax" : "FrameLib_ToHost",
        "FrameLib_MaxClass_FromMax" : "FrameLib_FromHost",
        "FrameLib_MaxClass_Info" : "FrameLib_Info",
        "FrameLib_MaxClass_Read" : "FrameLib_Read",
        "FrameLib_MaxClass_Sink" : "FrameLib_Sink",
        "FrameLib_MaxClass_ComplexExpression" : "FrameLib_ComplexExpression",
        "FrameLib_MaxClass_Expression" : "FrameLib_Expression",
        "FrameLib_MaxClass_ContextControl": "FrameLib_ContextControl"
    }
    if name in lookup:
        return lookup[name]
    return name

ignored_objects = [x for x in sys.argv[1:]]

def main(docs: Documentation):
    # Create the Max_Object_list.h and add skeleton
    op.write('#include "FrameLib_TypeList.h"\n\n')
    op.write("enum MaxObjectArgsMode { kAsParams, kAllInputs, kDistribute };\n\n")

    op.write("using FrameLib_DSPList = detail::FrameLib_Typelist<\n\n")

    # Category list formation

    source_file_list = []
    ## Get sources files and store with their category
    for file in docs.source_files:
        if file.stem not in ignored_objects:
            source_file_list.append([file.parent.stem, file])

    # TODO - lookwithin and realise this is not okay
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
    op.write(">;\n\n")

    ## Start const char bit
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
                    "const char* FrameLib_ObjectInfo<FrameLib_Expand<"
                    + fl_object_name
                    + '>>::name()\n{ return "'
                    + name.stem
                    + '"; }\n'
                )

            elif "_Expand" not in search_area and "makeClass" in search_area:
                op.write(
                    "const char* FrameLib_ObjectInfo<FrameLib_Expand<"
                    + fl_object_name
                    + '>>::name()\n{ return "'
                    + name.stem
                    + '"; }\n'
                )

            elif "_Expand" not in search_area and "makeClass" not in search_area:
                op.write(
                    "const char* FrameLib_ObjectInfo<"
                    + fl_object_name
                    + '>::name()\n{ return "'
                    + name.stem
                    + '"; }\n'
                )
            op.write("\n")

    ## Start argument type bit
    for category_folder, name in source_file_list:
        with open(Path(category_folder) / name, "r") as cpp:

            source_file = cpp.read().replace("\n", "").replace(" ", "")  # flatten it with no spaces whatsoever
            search_area = source_file.split('extern"C"intC74_EXPORTmain(void){')[1]

            fl_object_name = name_sanitisation(search_area.split("<")[1])
            arg_type = "kAsParams"

            if "kAllInputs" in source_file:
                arg_type = "kAllInputs"
            elif "kDistribute" in source_file:
                arg_type = "kDistribute"

            search_area = search_area.split("<")[0]
            # infer type with brutal checking by looking at text in the extern bit (search area)
            op.write("template<> template<> \n")
            if "_Expand" in search_area:
                op.write(
                    "MaxObjectArgsMode FrameLib_ObjectInfo<FrameLib_Expand<"
                    + fl_object_name
                    + ">>::option<MaxObjectArgsMode, 0>()\n{ return "
                    + arg_type
                    + "; }\n"
                )

            elif "_Expand" not in search_area and "makeClass" in search_area:
                op.write(
                    "MaxObjectArgsMode FrameLib_ObjectInfo<FrameLib_Expand<"
                    + fl_object_name
                    + ">>::option<MaxObjectArgsMode, 0>()\n{ return "
                    + arg_type
                    + "; }\n"
                )

            elif "_Expand" not in search_area and "makeClass" not in search_area:
                op.write(
                    "MaxObjectArgsMode FrameLib_ObjectInfo<"
                    + fl_object_name
                    + ">::option<MaxObjectArgsMode, 0>()\n{ return "
                    + arg_type
                    + "; }\n"
                )
            op.write("\n")

    op.close()

if __name__ == "__main__":
    main(Documentation())
