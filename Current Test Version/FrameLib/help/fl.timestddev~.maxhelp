{
    "patcher": {
        "fileversion": 1,
        "appversion": {
            "major": 8,
            "minor": 1,
            "revision": 0,
            "architecture": "x64",
            "modernui": 1
        },
        "classnamespace": "box",
        "rect": [
            34.0,
            79.0,
            662.0,
            787.0
        ],
        "bglocked": 0,
        "openinpresentation": 0,
        "default_fontsize": 12.0,
        "default_fontface": 0,
        "default_fontname": "Arial",
        "gridonopen": 1,
        "gridsize": [
            15.0,
            15.0
        ],
        "gridsnaponopen": 1,
        "objectsnaponopen": 1,
        "statusbarvisible": 2,
        "toolbarvisible": 1,
        "lefttoolbarpinned": 0,
        "toptoolbarpinned": 0,
        "righttoolbarpinned": 0,
        "bottomtoolbarpinned": 0,
        "toolbars_unpinned_last_save": 0,
        "tallnewobj": 0,
        "boxanimatetime": 200,
        "enablehscroll": 1,
        "enablevscroll": 1,
        "devicewidth": 0.0,
        "description": "",
        "digest": "",
        "tags": "",
        "style": "",
        "subpatcher_template": "nobars",
        "showrootpatcherontab": 0,
        "showontab": 0,
        "boxes": [
            {
                "box": {
                    "id": "obj-2",
                    "maxclass": "newobj",
                    "numinlets": 0,
                    "numoutlets": 0,
                    "patcher": {
                        "fileversion": 1,
                        "appversion": {
                            "major": 8,
                            "minor": 1,
                            "revision": 0,
                            "architecture": "x64",
                            "modernui": 1
                        },
                        "classnamespace": "box",
                        "rect": [
                            34.0,
                            105.0,
                            662.0,
                            761.0
                        ],
                        "bglocked": 0,
                        "openinpresentation": 0,
                        "default_fontsize": 12.0,
                        "default_fontface": 0,
                        "default_fontname": "Arial",
                        "gridonopen": 2,
                        "gridsize": [
                            15.0,
                            15.0
                        ],
                        "gridsnaponopen": 2,
                        "objectsnaponopen": 1,
                        "statusbarvisible": 2,
                        "toolbarvisible": 1,
                        "lefttoolbarpinned": 0,
                        "toptoolbarpinned": 0,
                        "righttoolbarpinned": 0,
                        "bottomtoolbarpinned": 0,
                        "toolbars_unpinned_last_save": 0,
                        "tallnewobj": 0,
                        "boxanimatetime": 200,
                        "enablehscroll": 1,
                        "enablevscroll": 1,
                        "devicewidth": 0.0,
                        "description": "",
                        "digest": "",
                        "tags": "",
                        "style": "",
                        "subpatcher_template": "nobars",
                        "showontab": 1,
                        "boxes": [
                            {
                                "box": {
                                    "hidden": 1,
                                    "id": "obj-1",
                                    "linecount": 7,
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        "bang"
                                    ],
                                    "patching_rect": [
                                        626.0,
                                        63.0,
                                        19.0,
                                        102.0
                                    ],
                                    "text": "loadbang"
                                }
                            },
                            {
                                "box": {
                                    "border": 0,
                                    "filename": "fl.helpdetails.js",
                                    "id": "obj-4",
                                    "ignoreclick": 1,
                                    "jsarguments": "fl.timestddev~",
                                    "maxclass": "jsui",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "parameter_enable": 0,
                                    "patching_rect": [
                                        15.0,
                                        15.0,
                                        630.0,
                                        150.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "bgmode": 0,
                                    "border": 0,
                                    "clickthrough": 0,
                                    "enablehscroll": 0,
                                    "enablevscroll": 0,
                                    "id": "obj-128",
                                    "lockeddragscroll": 0,
                                    "maxclass": "bpatcher",
                                    "name": "fldocs.getparams.maxpat",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "offset": [
                                        0.0,
                                        0.0
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        167.0,
                                        630.0,
                                        565.0
                                    ],
                                    "viewvisibility": 1
                                }
                            }
                        ],
                        "lines": [
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-128",
                                        0
                                    ],
                                    "hidden": 1,
                                    "midpoints": [
                                        635.5,
                                        168.0,
                                        657.0,
                                        168.0,
                                        657.0,
                                        166.0,
                                        24.5,
                                        166.0
                                    ],
                                    "source": [
                                        "obj-1",
                                        0
                                    ]
                                }
                            }
                        ]
                    },
                    "patching_rect": [
                        2.0,
                        5.0,
                        38.0,
                        22.0
                    ],
                    "saved_object_attributes": {
                        "description": "",
                        "digest": "",
                        "globalpatchername": "",
                        "tags": ""
                    },
                    "text": "p info"
                }
            },
            {
                "box": {
                    "id": "obj-2",
                    "maxclass": "newobj",
                    "numinlets": 0,
                    "numoutlets": 0,
                    "patcher": {
                        "fileversion": 1,
                        "appversion": {
                            "major": 8,
                            "minor": 1,
                            "revision": 3,
                            "architecture": "x64",
                            "modernui": 1
                        },
                        "classnamespace": "box",
                        "rect": [
                            0.0,
                            26.0,
                            652.0,
                            761.0
                        ],
                        "bglocked": 0,
                        "openinpresentation": 0,
                        "default_fontsize": 12.0,
                        "default_fontface": 0,
                        "default_fontname": "Arial",
                        "gridonopen": 1,
                        "gridsize": [
                            15.0,
                            15.0
                        ],
                        "gridsnaponopen": 1,
                        "objectsnaponopen": 1,
                        "statusbarvisible": 2,
                        "toolbarvisible": 1,
                        "lefttoolbarpinned": 0,
                        "toptoolbarpinned": 0,
                        "righttoolbarpinned": 0,
                        "bottomtoolbarpinned": 0,
                        "toolbars_unpinned_last_save": 0,
                        "tallnewobj": 0,
                        "boxanimatetime": 200,
                        "enablehscroll": 1,
                        "enablevscroll": 1,
                        "devicewidth": 0.0,
                        "description": "",
                        "digest": "",
                        "tags": "",
                        "style": "",
                        "subpatcher_template": "",
                        "showontab": 1,
                        "boxes": [],
                        "lines": []
                    },
                    "patching_rect": [
                        540.0,
                        230.0,
                        25.0,
                        22.0
                    ],
                    "saved_object_attributes": {
                        "description": "",
                        "digest": "",
                        "globalpatchername": "",
                        "tags": ""
                    },
                    "text": "p ?"
                }
            },
            {
                "box": {
                    "id": "obj-1",
                    "maxclass": "newobj",
                    "numinlets": 0,
                    "numoutlets": 0,
                    "patcher": {
                        "fileversion": 1,
                        "appversion": {
                            "major": 8,
                            "minor": 1,
                            "revision": 3,
                            "architecture": "x64",
                            "modernui": 1
                        },
                        "classnamespace": "box",
                        "rect": [
                            34.0,
                            105.0,
                            652.0,
                            761.0
                        ],
                        "bglocked": 0,
                        "openinpresentation": 0,
                        "default_fontsize": 13.0,
                        "default_fontface": 0,
                        "default_fontname": "Arial",
                        "gridonopen": 2,
                        "gridsize": [
                            15.0,
                            15.0
                        ],
                        "gridsnaponopen": 2,
                        "objectsnaponopen": 1,
                        "statusbarvisible": 2,
                        "toolbarvisible": 1,
                        "lefttoolbarpinned": 0,
                        "toptoolbarpinned": 0,
                        "righttoolbarpinned": 0,
                        "bottomtoolbarpinned": 0,
                        "toolbars_unpinned_last_save": 0,
                        "tallnewobj": 0,
                        "boxanimatetime": 200,
                        "enablehscroll": 1,
                        "enablevscroll": 1,
                        "devicewidth": 0.0,
                        "description": "",
                        "digest": "",
                        "tags": "",
                        "style": "",
                        "subpatcher_template": "",
                        "showontab": 1,
                        "boxes": [
                            {
                                "box": {
                                    "id": "obj-9",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        128.0,
                                        420.0,
                                        82.0,
                                        23.0
                                    ],
                                    "text": "loadmess 50"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-8",
                                    "linecount": 2,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        185.0,
                                        450.0,
                                        315.0,
                                        36.0
                                    ],
                                    "text": "amount of frames to calculate standard deviation over"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-7",
                                    "maxclass": "number",
                                    "numinlets": 1,
                                    "numoutlets": 2,
                                    "outlettype": [
                                        "",
                                        "bang"
                                    ],
                                    "parameter_enable": 0,
                                    "patching_rect": [
                                        128.0,
                                        450.0,
                                        50.0,
                                        23.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-4",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        128.0,
                                        495.0,
                                        99.0,
                                        23.0
                                    ],
                                    "text": "num_frames $1"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-1",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        128.0,
                                        540.0,
                                        124.0,
                                        23.0
                                    ],
                                    "text": "fl.frommax~ params"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-6",
                                    "linecount": 2,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        450.0,
                                        184.5,
                                        175.0,
                                        36.0
                                    ],
                                    "text": "Wiggle the multislider to create some changes"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-39",
                                    "linecount": 4,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        315.0,
                                        665.0,
                                        255.0,
                                        65.0
                                    ],
                                    "text": "The output here is the standard deviation, so the shape of this output here is a quasi-measurement of change for each index between frames"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-38",
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        149.0,
                                        580.0,
                                        301.0,
                                        21.0
                                    ],
                                    "text": "Take the standard deviation of the last 50 frames"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-36",
                                    "linecount": 2,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        195.0,
                                        342.0,
                                        175.0,
                                        36.0
                                    ],
                                    "text": "Every 1024 samples sample the multislider's output"
                                }
                            },
                            {
                                "box": {
                                    "contdata": 1,
                                    "ghostbar": 100,
                                    "id": "obj-32",
                                    "maxclass": "multislider",
                                    "numinlets": 1,
                                    "numoutlets": 2,
                                    "outlettype": [
                                        "",
                                        ""
                                    ],
                                    "parameter_enable": 0,
                                    "patching_rect": [
                                        15.0,
                                        645.0,
                                        285.0,
                                        105.0
                                    ],
                                    "setminmax": [
                                        0.0,
                                        1.0
                                    ],
                                    "size": 500,
                                    "slidercolor": [
                                        0.501960784313725,
                                        0.976470588235294,
                                        0.470588235294118,
                                        1.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-31",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        60.0,
                                        330.0,
                                        101.0,
                                        23.0
                                    ],
                                    "text": "fl.interval~ 1024"
                                }
                            },
                            {
                                "box": {
                                    "contdata": 1,
                                    "ghostbar": 100,
                                    "id": "obj-29",
                                    "maxclass": "multislider",
                                    "numinlets": 1,
                                    "numoutlets": 2,
                                    "outlettype": [
                                        "",
                                        ""
                                    ],
                                    "parameter_enable": 0,
                                    "patching_rect": [
                                        15.0,
                                        105.0,
                                        420.0,
                                        195.0
                                    ],
                                    "size": 500,
                                    "slidercolor": [
                                        0.466666666666667,
                                        0.964705882352941,
                                        0.972549019607843,
                                        1.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-28",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        375.0,
                                        77.0,
                                        23.0
                                    ],
                                    "text": "fl.frommax~"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-26",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        615.0,
                                        62.0,
                                        23.0
                                    ],
                                    "text": "fl.tomax~"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-13",
                                    "maxclass": "newobj",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        580.0,
                                        132.0,
                                        23.0
                                    ],
                                    "text": "fl.timestddev~ 100 50"
                                }
                            },
                            {
                                "box": {
                                    "border": 0,
                                    "filename": "fl.helpname.js",
                                    "id": "obj-3",
                                    "ignoreclick": 1,
                                    "jsarguments": [
                                        "fl.timestddev~"
                                    ],
                                    "maxclass": "jsui",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "parameter_enable": 0,
                                    "patching_rect": [
                                        15.0,
                                        15.0,
                                        435.0,
                                        75.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "angle": 270.0,
                                    "background": 1,
                                    "bgcolor": [
                                        0.2,
                                        0.2,
                                        0.2,
                                        0.0
                                    ],
                                    "border": 2,
                                    "bordercolor": [
                                        0.501960784313725,
                                        0.501960784313725,
                                        0.501960784313725,
                                        1.0
                                    ],
                                    "id": "obj-37",
                                    "maxclass": "panel",
                                    "mode": 0,
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        7.0,
                                        315.0,
                                        186.0,
                                        90.0
                                    ],
                                    "proportion": 0.5
                                }
                            }
                        ],
                        "lines": [
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-13",
                                        1
                                    ],
                                    "source": [
                                        "obj-1",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-26",
                                        0
                                    ],
                                    "source": [
                                        "obj-13",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-32",
                                        0
                                    ],
                                    "source": [
                                        "obj-26",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-1",
                                        0
                                    ],
                                    "midpoints": [
                                        24.5,
                                        534.0,
                                        137.5,
                                        534.0
                                    ],
                                    "order": 0,
                                    "source": [
                                        "obj-28",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-13",
                                        0
                                    ],
                                    "order": 1,
                                    "source": [
                                        "obj-28",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-28",
                                        0
                                    ],
                                    "source": [
                                        "obj-29",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-28",
                                        0
                                    ],
                                    "midpoints": [
                                        69.5,
                                        363.5,
                                        24.5,
                                        363.5
                                    ],
                                    "source": [
                                        "obj-31",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-1",
                                        0
                                    ],
                                    "source": [
                                        "obj-4",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-4",
                                        0
                                    ],
                                    "source": [
                                        "obj-7",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-7",
                                        0
                                    ],
                                    "source": [
                                        "obj-9",
                                        0
                                    ]
                                }
                            }
                        ]
                    },
                    "patching_rect": [
                        271.0,
                        230.0,
                        47.0,
                        22.0
                    ],
                    "saved_object_attributes": {
                        "description": "",
                        "digest": "",
                        "fontsize": 13.0,
                        "globalpatchername": "",
                        "tags": ""
                    },
                    "text": "p basic"
                }
            }
        ],
        "lines": [],
        "dependency_cache": [
            {
                "name": "fldocs.getparams.maxpat",
                "bootpath": "~/dev/FrameLib/Current Test Version/FrameLib/misc",
                "patcherrelativepath": "../../../Current Test Version/FrameLib/misc",
                "type": "JSON",
                "implicit": 1
            },
            {
                "name": "fl.getname.js",
                "bootpath": "~/dev/FrameLib/Current Test Version/FrameLib/misc",
                "patcherrelativepath": "../../../Current Test Version/FrameLib/misc",
                "type": "TEXT",
                "implicit": 1
            },
            {
                "name": "fl.helpdetails.js",
                "bootpath": "~/dev/FrameLib/Current Test Version/FrameLib/misc",
                "patcherrelativepath": "../../../Current Test Version/FrameLib/misc",
                "type": "TEXT",
                "implicit": 1
            }
        ],
        "autosave": 0
    }
}