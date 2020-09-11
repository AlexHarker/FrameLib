from FrameLibDocs.utils import read_json, write_json
from FrameLibDocs.variables import help_dir, current_version



def main():
    template_dir = help_dir / "templates"
    internal_dir = help_dir / "internal_tabs"
    external_dir = current_version / "FrameLib" /  "externals"
    master_template = help_dir / "help_template.maxhelp"

    templates = [x for x in template_dir.rglob("fl.*.maxhelp")]
    for t in templates:
        template = read_json(t)
        tabs = read_json(internal_dir / t.name)
        tabs_boxes = tabs['patcher']['boxes']
        for box in tabs_boxes:
            template['patcher']['boxes'].append(box)
            
            write_json(
                template_dir / t.name,
                template, 
            )

if __name__ == "__main__":
    main()


    













