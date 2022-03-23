from framelib.utils import read_json, write_json
from framelib.classes import Documentation

def main(docs: Documentation):
    template_dir = docs.help_dir / "templates"
    internal_dir = docs.help_dir / "internal_tabs"
    external_dir = docs.package / "FrameLib" / "externals"
    master_template = docs.help_dir / "help_template.maxhelp"

    templates = [x for x in template_dir.rglob("fl.*.maxhelp")]
    for t in templates:
        try:
            template = read_json(t)
            tabs = read_json(internal_dir / t.name)
        except FileNotFoundError:
            print(f'Ignoring {t.stem} wthout internal tabs')
        else:
            tabs_boxes = tabs["patcher"]["boxes"]
            for box in tabs_boxes:
                template["patcher"]["boxes"].append(box)
                write_json(template_dir / t.name, template)


if __name__ == "__main__":
    main(Documentation())
