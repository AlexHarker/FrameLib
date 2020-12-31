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
                                    "jsarguments": "fl.complex.expr~",
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
                    "id": "obj-4",
                    "maxclass": "newobj",
                    "numinlets": 0,
                    "numoutlets": 0,
                    "patcher": {
                        "fileversion": 1,
                        "appversion": {
                            "major": 8,
                            "minor": 1,
                            "revision": 8,
                            "architecture": "x64",
                            "modernui": 1
                        },
                        "classnamespace": "box",
                        "rect": [
                            34.0,
                            109.0,
                            653.0,
                            757.0
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
                        "assistshowspatchername": 0,
                        "boxes": [
                            {
                                "box": {
                                    "hidden": 1,
                                    "id": "obj-5",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        345.0,
                                        85.0,
                                        23.0
                                    ],
                                    "text": "loadmess set"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-2",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        219.0,
                                        315.0,
                                        191.0,
                                        23.0
                                    ],
                                    "presentation_linecount": 2,
                                    "text": "-0.80768 -0.500645 -18.334721"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-4",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        219.0,
                                        270.0,
                                        62.0,
                                        23.0
                                    ],
                                    "text": "fl.tomax~"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-1",
                                    "maxclass": "newobj",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        219.0,
                                        180.0,
                                        106.0,
                                        23.0
                                    ],
                                    "text": "fl.register~ -6 6 4"
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Arial",
                                    "id": "obj-6",
                                    "linecount": 2,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        15.0,
                                        90.0,
                                        617.0,
                                        36.0
                                    ],
                                    "text": "Several functions are supported. A full list of avilable functions and operators can be retrieved with the info message."
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-67",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        315.0,
                                        183.0,
                                        23.0
                                    ],
                                    "text": "2.643328 0.438905 -3.473516"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-64",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        270.0,
                                        62.0,
                                        23.0
                                    ],
                                    "text": "fl.tomax~"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-63",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        135.0,
                                        54.0,
                                        23.0
                                    ],
                                    "text": "fl.once~"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-62",
                                    "maxclass": "newobj",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        180.0,
                                        102.0,
                                        23.0
                                    ],
                                    "text": "fl.register~ 1 0 3"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-54",
                                    "maxclass": "newobj",
                                    "numinlets": 2,
                                    "numoutlets": 2,
                                    "outlettype": [
                                        "",
                                        ""
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        225.0,
                                        223.0,
                                        23.0
                                    ],
                                    "text": "fl.complex.expr~ sinh(in1) * log(in1)"
                                }
                            },
                            {
                                "box": {
                                    "border": 0,
                                    "filename": "fl.helpname.js",
                                    "id": "obj-3",
                                    "ignoreclick": 1,
                                    "jsarguments": [
                                        "fl.complex.expr~"
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
                            }
                        ],
                        "lines": [
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-54",
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
                                        "obj-2",
                                        1
                                    ],
                                    "midpoints": [
                                        228.5,
                                        303.5,
                                        400.5,
                                        303.5
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
                                        "obj-2",
                                        0
                                    ],
                                    "hidden": 1,
                                    "order": 0,
                                    "source": [
                                        "obj-5",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-67",
                                        0
                                    ],
                                    "hidden": 1,
                                    "order": 1,
                                    "source": [
                                        "obj-5",
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
                                        "obj-54",
                                        1
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-64",
                                        0
                                    ],
                                    "source": [
                                        "obj-54",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-54",
                                        0
                                    ],
                                    "source": [
                                        "obj-62",
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
                                        168.5,
                                        228.5,
                                        168.5
                                    ],
                                    "order": 0,
                                    "source": [
                                        "obj-63",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-62",
                                        0
                                    ],
                                    "order": 1,
                                    "source": [
                                        "obj-63",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-67",
                                        1
                                    ],
                                    "midpoints": [
                                        24.5,
                                        303.5,
                                        188.5,
                                        303.5
                                    ],
                                    "source": [
                                        "obj-64",
                                        0
                                    ]
                                }
                            }
                        ],
                        "styles": [
                            {
                                "name": "AudioStatus_Menu",
                                "default": {
                                    "bgfillcolor": {
                                        "type": "color",
                                        "color": [
                                            0.294118,
                                            0.313726,
                                            0.337255,
                                            1
                                        ],
                                        "color1": [
                                            0.454902,
                                            0.462745,
                                            0.482353,
                                            0.0
                                        ],
                                        "color2": [
                                            0.290196,
                                            0.309804,
                                            0.301961,
                                            1.0
                                        ],
                                        "angle": 270.0,
                                        "proportion": 0.39,
                                        "autogradient": 0
                                    }
                                },
                                "parentstyle": "",
                                "multi": 0
                            },
                            {
                                "name": "newobjBlue-1",
                                "default": {
                                    "accentcolor": [
                                        0.317647,
                                        0.654902,
                                        0.976471,
                                        1.0
                                    ]
                                },
                                "parentstyle": "",
                                "multi": 0
                            },
                            {
                                "name": "newobjBrown-1",
                                "default": {
                                    "accentcolor": [
                                        0.654902,
                                        0.572549,
                                        0.376471,
                                        1.0
                                    ]
                                },
                                "parentstyle": "",
                                "multi": 0
                            },
                            {
                                "name": "newobjGreen-1",
                                "default": {
                                    "accentcolor": [
                                        0.0,
                                        0.533333,
                                        0.168627,
                                        1.0
                                    ]
                                },
                                "parentstyle": "",
                                "multi": 0
                            },
                            {
                                "name": "newobjRed-1",
                                "default": {
                                    "accentcolor": [
                                        0.784314,
                                        0.145098,
                                        0.023529,
                                        1.0
                                    ]
                                },
                                "parentstyle": "",
                                "multi": 0
                            },
                            {
                                "name": "newobjYellow-1",
                                "default": {
                                    "accentcolor": [
                                        0.82517,
                                        0.78181,
                                        0.059545,
                                        1.0
                                    ],
                                    "fontsize": [
                                        12.059008
                                    ]
                                },
                                "parentstyle": "",
                                "multi": 0
                            },
                            {
                                "name": "numberGold-1",
                                "default": {
                                    "accentcolor": [
                                        0.764706,
                                        0.592157,
                                        0.101961,
                                        1.0
                                    ]
                                },
                                "parentstyle": "",
                                "multi": 0
                            },
                            {
                                "name": "panelViolet",
                                "default": {
                                    "bgfillcolor": {
                                        "type": "color",
                                        "color": [
                                            0.372549,
                                            0.196078,
                                            0.486275,
                                            0.2
                                        ],
                                        "color1": [
                                            0.454902,
                                            0.462745,
                                            0.482353,
                                            1.0
                                        ],
                                        "color2": [
                                            0.290196,
                                            0.309804,
                                            0.301961,
                                            1.0
                                        ],
                                        "angle": 270.0,
                                        "proportion": 0.39,
                                        "autogradient": 0
                                    }
                                },
                                "parentstyle": "",
                                "multi": 0
                            }
                        ]
                    },
                    "patching_rect": [
                        420.0,
                        230.0,
                        67.0,
                        22.0
                    ],
                    "saved_object_attributes": {
                        "description": "",
                        "digest": "",
                        "fontsize": 13.0,
                        "globalpatchername": "",
                        "tags": ""
                    },
                    "text": "p functions"
                }
            },
            {
                "box": {
                    "id": "obj-3",
                    "maxclass": "newobj",
                    "numinlets": 0,
                    "numoutlets": 0,
                    "patcher": {
                        "fileversion": 1,
                        "appversion": {
                            "major": 8,
                            "minor": 1,
                            "revision": 8,
                            "architecture": "x64",
                            "modernui": 1
                        },
                        "classnamespace": "box",
                        "rect": [
                            0.0,
                            26.0,
                            653.0,
                            757.0
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
                        "assistshowspatchername": 0,
                        "boxes": [
                            {
                                "box": {
                                    "id": "obj-7",
                                    "linecount": 2,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        75.0,
                                        390.0,
                                        552.0,
                                        36.0
                                    ],
                                    "presentation_linecount": 6,
                                    "text": "Note that where subexpressions evaluate as constant fl.complex.expr~ will pre-calculate them (for example, above the whole expression is constant)."
                                }
                            },
                            {
                                "box": {
                                    "arrows": 1,
                                    "border": 4.0,
                                    "id": "obj-30",
                                    "maxclass": "live.line",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        203.0,
                                        230.0,
                                        35.0,
                                        13.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-6",
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        240.0,
                                        225.0,
                                        174.0,
                                        21.0
                                    ],
                                    "text": "Remember Euler's theorem?"
                                }
                            },
                            {
                                "box": {
                                    "format": 6,
                                    "id": "obj-2",
                                    "maxclass": "flonum",
                                    "numinlets": 1,
                                    "numoutlets": 2,
                                    "outlettype": [
                                        "",
                                        "bang"
                                    ],
                                    "parameter_enable": 0,
                                    "patching_rect": [
                                        175.0,
                                        315.0,
                                        50.0,
                                        23.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-4",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        175.0,
                                        270.0,
                                        75.0,
                                        23.0
                                    ],
                                    "text": "fl.tomax~"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-1",
                                    "local": 1,
                                    "maxclass": "ezdac~",
                                    "numinlets": 2,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        15.0,
                                        375.0,
                                        45.0,
                                        45.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-40",
                                    "maxclass": "newobj",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        180.0,
                                        80.0,
                                        23.0
                                    ],
                                    "text": "fl.register~ 1"
                                }
                            },
                            {
                                "box": {
                                    "format": 6,
                                    "id": "obj-39",
                                    "maxclass": "flonum",
                                    "numinlets": 1,
                                    "numoutlets": 2,
                                    "outlettype": [
                                        "",
                                        "bang"
                                    ],
                                    "parameter_enable": 0,
                                    "patching_rect": [
                                        15.0,
                                        315.0,
                                        50.0,
                                        23.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-37",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        270.0,
                                        75.0,
                                        23.0
                                    ],
                                    "text": "fl.tomax~"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-36",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        135.0,
                                        54.0,
                                        23.0
                                    ],
                                    "text": "fl.once~"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-35",
                                    "maxclass": "newobj",
                                    "numinlets": 2,
                                    "numoutlets": 2,
                                    "outlettype": [
                                        "",
                                        ""
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        225.0,
                                        179.0,
                                        23.0
                                    ],
                                    "text": "fl.complex.expr~ pow(e\\, i * pi)"
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Arial",
                                    "id": "obj-34",
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        15.0,
                                        90.0,
                                        545.0,
                                        21.0
                                    ],
                                    "text": "You can refer to constants in fl.complex expr~. Available constants are i, e, pi, epsilon and inf."
                                }
                            },
                            {
                                "box": {
                                    "border": 0,
                                    "filename": "fl.helpname.js",
                                    "id": "obj-3",
                                    "ignoreclick": 1,
                                    "jsarguments": [
                                        "fl.complex.expr~"
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
                            }
                        ],
                        "lines": [
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-37",
                                        0
                                    ],
                                    "source": [
                                        "obj-35",
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
                                        "obj-35",
                                        1
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-40",
                                        0
                                    ],
                                    "source": [
                                        "obj-36",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-39",
                                        0
                                    ],
                                    "source": [
                                        "obj-37",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-2",
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
                                        "obj-35",
                                        0
                                    ],
                                    "source": [
                                        "obj-40",
                                        0
                                    ]
                                }
                            }
                        ],
                        "styles": [
                            {
                                "name": "AudioStatus_Menu",
                                "default": {
                                    "bgfillcolor": {
                                        "type": "color",
                                        "color": [
                                            0.294118,
                                            0.313726,
                                            0.337255,
                                            1
                                        ],
                                        "color1": [
                                            0.454902,
                                            0.462745,
                                            0.482353,
                                            0.0
                                        ],
                                        "color2": [
                                            0.290196,
                                            0.309804,
                                            0.301961,
                                            1.0
                                        ],
                                        "angle": 270.0,
                                        "proportion": 0.39,
                                        "autogradient": 0
                                    }
                                },
                                "parentstyle": "",
                                "multi": 0
                            },
                            {
                                "name": "newobjBlue-1",
                                "default": {
                                    "accentcolor": [
                                        0.317647,
                                        0.654902,
                                        0.976471,
                                        1.0
                                    ]
                                },
                                "parentstyle": "",
                                "multi": 0
                            },
                            {
                                "name": "newobjBrown-1",
                                "default": {
                                    "accentcolor": [
                                        0.654902,
                                        0.572549,
                                        0.376471,
                                        1.0
                                    ]
                                },
                                "parentstyle": "",
                                "multi": 0
                            },
                            {
                                "name": "newobjGreen-1",
                                "default": {
                                    "accentcolor": [
                                        0.0,
                                        0.533333,
                                        0.168627,
                                        1.0
                                    ]
                                },
                                "parentstyle": "",
                                "multi": 0
                            },
                            {
                                "name": "newobjRed-1",
                                "default": {
                                    "accentcolor": [
                                        0.784314,
                                        0.145098,
                                        0.023529,
                                        1.0
                                    ]
                                },
                                "parentstyle": "",
                                "multi": 0
                            },
                            {
                                "name": "newobjYellow-1",
                                "default": {
                                    "accentcolor": [
                                        0.82517,
                                        0.78181,
                                        0.059545,
                                        1.0
                                    ],
                                    "fontsize": [
                                        12.059008
                                    ]
                                },
                                "parentstyle": "",
                                "multi": 0
                            },
                            {
                                "name": "numberGold-1",
                                "default": {
                                    "accentcolor": [
                                        0.764706,
                                        0.592157,
                                        0.101961,
                                        1.0
                                    ]
                                },
                                "parentstyle": "",
                                "multi": 0
                            },
                            {
                                "name": "panelViolet",
                                "default": {
                                    "bgfillcolor": {
                                        "type": "color",
                                        "color": [
                                            0.372549,
                                            0.196078,
                                            0.486275,
                                            0.2
                                        ],
                                        "color1": [
                                            0.454902,
                                            0.462745,
                                            0.482353,
                                            1.0
                                        ],
                                        "color2": [
                                            0.290196,
                                            0.309804,
                                            0.301961,
                                            1.0
                                        ],
                                        "angle": 270.0,
                                        "proportion": 0.39,
                                        "autogradient": 0
                                    }
                                },
                                "parentstyle": "",
                                "multi": 0
                            }
                        ]
                    },
                    "patching_rect": [
                        330.0,
                        230.0,
                        70.0,
                        22.0
                    ],
                    "saved_object_attributes": {
                        "description": "",
                        "digest": "",
                        "fontsize": 13.0,
                        "globalpatchername": "",
                        "tags": ""
                    },
                    "text": "p constants"
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
                            "revision": 8,
                            "architecture": "x64",
                            "modernui": 1
                        },
                        "classnamespace": "box",
                        "rect": [
                            0.0,
                            26.0,
                            653.0,
                            757.0
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
                        "assistshowspatchername": 0,
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
                            "revision": 8,
                            "architecture": "x64",
                            "modernui": 1
                        },
                        "classnamespace": "box",
                        "rect": [
                            0.0,
                            26.0,
                            653.0,
                            757.0
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
                        "assistshowspatchername": 0,
                        "boxes": [
                            {
                                "box": {
                                    "id": "obj-15",
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        15.0,
                                        525.0,
                                        282.0,
                                        21.0
                                    ],
                                    "text": "Inputs are in pairs or real and imaginary values."
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-14",
                                    "linecount": 2,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        15.0,
                                        480.0,
                                        471.0,
                                        36.0
                                    ],
                                    "presentation_linecount": 2,
                                    "text": "Inputs are referred to by in1, in2, in3 etc. As many as needed will be made once you've entered the expression."
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-12",
                                    "linecount": 2,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        15.0,
                                        435.0,
                                        548.0,
                                        36.0
                                    ],
                                    "text": "You can enter the expression directly and use (a + bi) notation to represent complex numbers as part of the expression (as above)."
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-1",
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        592.75,
                                        407.0,
                                        20.0,
                                        21.0
                                    ],
                                    "text": "-1"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-2",
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        592.75,
                                        357.0,
                                        19.0,
                                        21.0
                                    ],
                                    "text": "0"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-4",
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        592.75,
                                        307.0,
                                        19.0,
                                        21.0
                                    ],
                                    "text": "1"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-5",
                                    "justification": 1,
                                    "maxclass": "live.line",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        579.75,
                                        415.0,
                                        11.0,
                                        5.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-7",
                                    "justification": 1,
                                    "maxclass": "live.line",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        579.75,
                                        315.0,
                                        11.0,
                                        5.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-8",
                                    "justification": 1,
                                    "maxclass": "live.line",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        579.75,
                                        365.0,
                                        11.0,
                                        5.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-9",
                                    "maxclass": "multislider",
                                    "numinlets": 1,
                                    "numoutlets": 2,
                                    "outlettype": [
                                        "",
                                        ""
                                    ],
                                    "parameter_enable": 0,
                                    "patching_rect": [
                                        322.75,
                                        315.0,
                                        255.0,
                                        105.0
                                    ],
                                    "size": 100
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-10",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        322.75,
                                        270.0,
                                        62.0,
                                        23.0
                                    ],
                                    "text": "fl.tomax~"
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Arial",
                                    "id": "obj-6",
                                    "linecount": 2,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        15.0,
                                        90.0,
                                        616.0,
                                        36.0
                                    ],
                                    "text": "fl.coplex.expr~ allows you to create custom expressions in a single object contaning inputs, unary/binary/ternary operators and constants that operate on complex numbers."
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-67",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        870.0,
                                        225.0,
                                        23.0
                                    ],
                                    "text": "0.283185 -0.283185 -1.483185 3.1"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-64",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        825.0,
                                        62.0,
                                        23.0
                                    ],
                                    "text": "fl.tomax~"
                                }
                            },
                            {
                                "box": {
                                    "arrows": 1,
                                    "border": 4.0,
                                    "id": "obj-31",
                                    "maxclass": "live.line",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        219.25,
                                        228.0,
                                        31.25,
                                        17.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-32",
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        255.0,
                                        228.0,
                                        173.0,
                                        21.0
                                    ],
                                    "text": "Rotate the input by 0.5 * 0.2i"
                                }
                            },
                            {
                                "box": {
                                    "arrows": 1,
                                    "border": 4.0,
                                    "id": "obj-29",
                                    "maxclass": "live.line",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        247.75,
                                        183.0,
                                        31.25,
                                        17.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-30",
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        285.0,
                                        178.5,
                                        337.0,
                                        21.0
                                    ],
                                    "text": "Generate a ramp that sets both real and imaginary inputs"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-28",
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        285.0,
                                        407.0,
                                        20.0,
                                        21.0
                                    ],
                                    "text": "-1"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-27",
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        285.0,
                                        357.0,
                                        19.0,
                                        21.0
                                    ],
                                    "text": "0"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-26",
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        285.0,
                                        307.0,
                                        19.0,
                                        21.0
                                    ],
                                    "text": "1"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-24",
                                    "justification": 1,
                                    "maxclass": "live.line",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        272.0,
                                        415.0,
                                        11.0,
                                        5.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-23",
                                    "justification": 1,
                                    "maxclass": "live.line",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        272.0,
                                        315.0,
                                        11.0,
                                        5.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-22",
                                    "justification": 1,
                                    "maxclass": "live.line",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        272.0,
                                        365.0,
                                        11.0,
                                        5.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-21",
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
                                        315.0,
                                        255.0,
                                        105.0
                                    ],
                                    "size": 100
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-20",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        270.0,
                                        62.0,
                                        23.0
                                    ],
                                    "text": "fl.tomax~"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-18",
                                    "maxclass": "newobj",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        180.0,
                                        229.0,
                                        23.0
                                    ],
                                    "text": "fl.ramp~ /length 100 /scale normalised"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-16",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        135.0,
                                        54.0,
                                        23.0
                                    ],
                                    "text": "fl.once~"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-13",
                                    "maxclass": "newobj",
                                    "numinlets": 2,
                                    "numoutlets": 2,
                                    "outlettype": [
                                        "",
                                        ""
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        222.0,
                                        199.0,
                                        23.0
                                    ],
                                    "text": "fl.complex.expr~ in1 * (0.5 + 0.2i)"
                                }
                            },
                            {
                                "box": {
                                    "border": 0,
                                    "filename": "fl.helpname.js",
                                    "id": "obj-3",
                                    "ignoreclick": 1,
                                    "jsarguments": [
                                        "fl.complex.expr~"
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
                            }
                        ],
                        "lines": [
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-9",
                                        0
                                    ],
                                    "source": [
                                        "obj-10",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-10",
                                        0
                                    ],
                                    "midpoints": [
                                        204.5,
                                        258.5,
                                        332.25,
                                        258.5
                                    ],
                                    "source": [
                                        "obj-13",
                                        1
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-20",
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
                                        "obj-18",
                                        0
                                    ],
                                    "source": [
                                        "obj-16",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-13",
                                        1
                                    ],
                                    "midpoints": [
                                        24.5,
                                        212.0,
                                        204.5,
                                        212.0
                                    ],
                                    "order": 0,
                                    "source": [
                                        "obj-18",
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
                                        "obj-18",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-21",
                                        0
                                    ],
                                    "source": [
                                        "obj-20",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-67",
                                        1
                                    ],
                                    "source": [
                                        "obj-64",
                                        0
                                    ]
                                }
                            }
                        ],
                        "styles": [
                            {
                                "name": "AudioStatus_Menu",
                                "default": {
                                    "bgfillcolor": {
                                        "type": "color",
                                        "color": [
                                            0.294118,
                                            0.313726,
                                            0.337255,
                                            1
                                        ],
                                        "color1": [
                                            0.454902,
                                            0.462745,
                                            0.482353,
                                            0.0
                                        ],
                                        "color2": [
                                            0.290196,
                                            0.309804,
                                            0.301961,
                                            1.0
                                        ],
                                        "angle": 270.0,
                                        "proportion": 0.39,
                                        "autogradient": 0
                                    }
                                },
                                "parentstyle": "",
                                "multi": 0
                            },
                            {
                                "name": "newobjBlue-1",
                                "default": {
                                    "accentcolor": [
                                        0.317647,
                                        0.654902,
                                        0.976471,
                                        1.0
                                    ]
                                },
                                "parentstyle": "",
                                "multi": 0
                            },
                            {
                                "name": "newobjBrown-1",
                                "default": {
                                    "accentcolor": [
                                        0.654902,
                                        0.572549,
                                        0.376471,
                                        1.0
                                    ]
                                },
                                "parentstyle": "",
                                "multi": 0
                            },
                            {
                                "name": "newobjGreen-1",
                                "default": {
                                    "accentcolor": [
                                        0.0,
                                        0.533333,
                                        0.168627,
                                        1.0
                                    ]
                                },
                                "parentstyle": "",
                                "multi": 0
                            },
                            {
                                "name": "newobjRed-1",
                                "default": {
                                    "accentcolor": [
                                        0.784314,
                                        0.145098,
                                        0.023529,
                                        1.0
                                    ]
                                },
                                "parentstyle": "",
                                "multi": 0
                            },
                            {
                                "name": "newobjYellow-1",
                                "default": {
                                    "accentcolor": [
                                        0.82517,
                                        0.78181,
                                        0.059545,
                                        1.0
                                    ],
                                    "fontsize": [
                                        12.059008
                                    ]
                                },
                                "parentstyle": "",
                                "multi": 0
                            },
                            {
                                "name": "numberGold-1",
                                "default": {
                                    "accentcolor": [
                                        0.764706,
                                        0.592157,
                                        0.101961,
                                        1.0
                                    ]
                                },
                                "parentstyle": "",
                                "multi": 0
                            },
                            {
                                "name": "panelViolet",
                                "default": {
                                    "bgfillcolor": {
                                        "type": "color",
                                        "color": [
                                            0.372549,
                                            0.196078,
                                            0.486275,
                                            0.2
                                        ],
                                        "color1": [
                                            0.454902,
                                            0.462745,
                                            0.482353,
                                            1.0
                                        ],
                                        "color2": [
                                            0.290196,
                                            0.309804,
                                            0.301961,
                                            1.0
                                        ],
                                        "angle": 270.0,
                                        "proportion": 0.39,
                                        "autogradient": 0
                                    }
                                },
                                "parentstyle": "",
                                "multi": 0
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