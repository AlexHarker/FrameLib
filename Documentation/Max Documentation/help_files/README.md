# Help File Generators

This is mainly documentation for myself (James) to remember what to do.

The `internal_tabs` directory is a place to store all of the hardcoded tabs for each object's help file.

At any stage, you can rebuild the help files from the internal tabs in a three stage process:

1) Invoke the python REPL `python3`
2) Import the modules required

```
a = __import__("10_template_help")
b = __import__("11_mmti")
c = __import__("12_merge_help")
```
3) Then run the main function of each module in series

```
a.main()
b.main()
c.main()
```

Of course, all of this is automatically done as part of the larger build process but this is an easy way to quickly recompile separate parts of the docs.

If you need to change the master template it can be found at `help_file_template.maxhelp`