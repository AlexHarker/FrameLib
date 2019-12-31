# Help File Generators

This is mainly documentation for myself (James) to remember what to do.

The `internal_tabs` directory is a place to store all of the hardcoded tabs for each object's help file.

At any stage, you can run

`python3 help_file_templating.py` to generate the templates.

then:

`python3 merge_patchers.py`

which merges the internal tabs into the parent template to create a multi-tab help file.

If you need to change the master template it can be found at `help_file_template.maxhelp`