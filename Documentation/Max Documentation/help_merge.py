from framelib.utils import read_json, write_json
from framelib.classes import Documentation
from shutil import copyfile


def main(docs: Documentation):
    internal_dir = docs.help_dir / "internal_tabs"
    non_template_dir = docs.help_dir / "non_templated"

    templates = [x for x in docs.help_templates_dir.rglob("fl.*.maxhelp")]
    
    for t in templates:
        if (non_template_dir / t.name).exists():
            copyfile(non_template_dir / t.name, docs.help_templates_dir / t.name)
        else:
            try:
                template = read_json(t)
                tabs = read_json(internal_dir / t.name)
            except FileNotFoundError:
                print(f'Ignoring {t.stem} wthout internal tabs')
            else:
                tabs_boxes = tabs["patcher"]["boxes"]
                for box in tabs_boxes:
                    template["patcher"]["boxes"].append(box)
                    write_json(docs.help_templates_dir / t.name, template)


if __name__ == "__main__":
    main(Documentation())
