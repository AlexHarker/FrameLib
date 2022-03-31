from framelib.classes import Documentation
from shutil import copyfile


def main(docs: Documentation):
    master_template = docs.help_dir / "templates" / "help_template.maxhelp"

    # clean out templates folder
    for x in docs.help_templates_dir.rglob("fl.*.maxhelp"):
        x.unlink()

    for obj in docs.source_files:
        name = obj.stem
        help_path = docs.help_templates_dir / f"{name}.maxhelp"
        copyfile(master_template, help_path)
        

if __name__ == "__main__":
    main(Documentation())
