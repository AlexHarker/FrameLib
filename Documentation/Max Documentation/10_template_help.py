from FrameLibDocs.help import edit_help
from FrameLibDocs.variables import help_dir, current_version
from shutil import copyfile




def main():
    template_dir = help_dir / "templates"
    external_dir = current_version / "FrameLib" /  "externals"
    master_template = help_dir / "help_template.maxhelp"

    # clean out templates folder
    for x in template_dir.rglob("fl.*.maxhelp"):
        x.unlink()

    externals = [ 
        x 
        for g in ["*.mxo" ,"*.mxe64"] 
        for x in external_dir.rglob(g)
    ]
    for obj in externals:
        name = obj.stem
        help_path = template_dir / f'{name}.maxhelp'
        copyfile(master_template, help_path)
        edit_help(file_edit = help_path, obj_name = name)

if __name__ == "__main__":
    main()