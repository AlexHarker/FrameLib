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
                                    "jsarguments": "fl.clip~",
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
                                    "id": "obj-29",
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        148.0,
                                        572.0,
                                        409.0,
                                        21.0
                                    ],
                                    "text": "Observe how squared off the wave is from fl.clip~",
                                    "textjustification": 1
                                }
                            },
                            {
                                "box": {
                                    "arrows": 1,
                                    "border": 4.0,
                                    "id": "obj-27",
                                    "maxclass": "live.line",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        100.0,
                                        378.0,
                                        31.25,
                                        17.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-28",
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        133.25,
                                        375.0,
                                        436.0,
                                        21.0
                                    ],
                                    "text": "Get back some of the lost gain by clipping so harshly."
                                }
                            },
                            {
                                "box": {
                                    "arrows": 1,
                                    "border": 4.0,
                                    "id": "obj-25",
                                    "maxclass": "live.line",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        126.875,
                                        318.0,
                                        31.25,
                                        17.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-26",
                                    "linecount": 3,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        160.125,
                                        301.5,
                                        436.0,
                                        50.0
                                    ],
                                    "text": "Clip the audio at the values -0.2 and 0.2. This will quite harshly square off the audio if it raises above or below these values, resulting in a distorted sound effect."
                                }
                            },
                            {
                                "box": {
                                    "arrows": 1,
                                    "border": 4.0,
                                    "id": "obj-22",
                                    "maxclass": "live.line",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        272.0,
                                        216.0,
                                        31.25,
                                        17.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-23",
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        305.25,
                                        214.0,
                                        436.0,
                                        21.0
                                    ],
                                    "text": "Convert some audio playback stream into frames"
                                }
                            },
                            {
                                "box": {
                                    "arrows": 1,
                                    "border": 4.0,
                                    "id": "obj-16",
                                    "maxclass": "live.line",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        56.0,
                                        108.5,
                                        31.25,
                                        17.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-21",
                                    "linecount": 2,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        90.5,
                                        99.0,
                                        436.0,
                                        36.0
                                    ],
                                    "text": "Start the audio. Careful! We are clipping some audio on purpose to create a distorted effect. Make sure your volume is at an appropriate level."
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-17",
                                    "maxclass": "newobj",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        30.0,
                                        375.0,
                                        68.0,
                                        23.0
                                    ],
                                    "text": "fl.times~ 2"
                                }
                            },
                            {
                                "box": {
                                    "bufsize": 64,
                                    "id": "obj-13",
                                    "maxclass": "scope~",
                                    "numinlets": 2,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        150.0,
                                        495.0,
                                        405.0,
                                        75.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-12",
                                    "maxclass": "toggle",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        "int"
                                    ],
                                    "parameter_enable": 0,
                                    "patching_rect": [
                                        30.0,
                                        105.0,
                                        24.0,
                                        24.0
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-10",
                                    "maxclass": "newobj",
                                    "numinlets": 2,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        30.0,
                                        615.0,
                                        59.0,
                                        23.0
                                    ],
                                    "text": "dac~ 1 2"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-9",
                                    "maxclass": "newobj",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        "signal"
                                    ],
                                    "patching_rect": [
                                        30.0,
                                        435.0,
                                        49.0,
                                        23.0
                                    ],
                                    "text": "fl.sink~"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-7",
                                    "maxclass": "newobj",
                                    "numinlets": 2,
                                    "numoutlets": 2,
                                    "outlettype": [
                                        "signal",
                                        "bang"
                                    ],
                                    "patching_rect": [
                                        30.0,
                                        165.0,
                                        224.0,
                                        23.0
                                    ],
                                    "saved_object_attributes": {
                                        "basictuning": 440,
                                        "followglobaltempo": 0,
                                        "formantcorrection": 0,
                                        "mode": "basic",
                                        "originallength": [
                                            8501.46394557823,
                                            "ticks"
                                        ],
                                        "originaltempo": 120.0,
                                        "pitchcorrection": 0,
                                        "quality": "basic",
                                        "timestretch": [
                                            0
                                        ]
                                    },
                                    "text": "sfplay~ @audiofile duduk.aif @loop 1"
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
                                        90.5,
                                        210.0,
                                        101.0,
                                        23.0
                                    ],
                                    "text": "fl.interval~ 1024"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-2",
                                    "maxclass": "newobj",
                                    "numinlets": 3,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        30.0,
                                        255.0,
                                        140.0,
                                        23.0
                                    ],
                                    "text": "fl.source~ /length 1024"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-5",
                                    "maxclass": "newobj",
                                    "numinlets": 3,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        30.0,
                                        315.0,
                                        93.0,
                                        23.0
                                    ],
                                    "text": "fl.clip~ -0.2 0.2"
                                }
                            },
                            {
                                "box": {
                                    "border": 0,
                                    "filename": "fl.helpname.js",
                                    "id": "obj-3",
                                    "ignoreclick": 1,
                                    "jsarguments": [
                                        "fl.clip~"
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
                                        0.0,
                                        0.0,
                                        0.0,
                                        1.0
                                    ],
                                    "id": "obj-24",
                                    "maxclass": "panel",
                                    "mode": 0,
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        15.0,
                                        157.5,
                                        255.0,
                                        127.5
                                    ],
                                    "proportion": 0.5
                                }
                            }
                        ],
                        "lines": [
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-2",
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
                                        "obj-7",
                                        0
                                    ],
                                    "source": [
                                        "obj-12",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-9",
                                        0
                                    ],
                                    "source": [
                                        "obj-17",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-5",
                                        0
                                    ],
                                    "source": [
                                        "obj-2",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-17",
                                        0
                                    ],
                                    "source": [
                                        "obj-5",
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
                                        "obj-7",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-10",
                                        1
                                    ],
                                    "midpoints": [
                                        39.5,
                                        600.0,
                                        79.5,
                                        600.0
                                    ],
                                    "order": 1,
                                    "source": [
                                        "obj-9",
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
                                        39.5,
                                        459.0,
                                        39.5,
                                        459.0
                                    ],
                                    "order": 2,
                                    "source": [
                                        "obj-9",
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
                                    "midpoints": [
                                        39.5,
                                        480.0,
                                        159.5,
                                        480.0
                                    ],
                                    "order": 0,
                                    "source": [
                                        "obj-9",
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
                        "assistshowspatchername": 0,
                        "boxes": [
                            {
                                "box": {
                                    "id": "obj-6",
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        90.52499389648438,
                                        496.0,
                                        230.0,
                                        21.0
                                    ],
                                    "text": "The internal operation is a subtraction."
                                }
                            },
                            {
                                "box": {
                                    "arrows": 1,
                                    "border": 4.0,
                                    "id": "obj-37",
                                    "maxclass": "live.line",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        139.0,
                                        559.8999938964844,
                                        53.04998779296875,
                                        13.20001220703125
                                    ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-18",
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        195.0,
                                        555.0,
                                        207.0,
                                        21.0
                                    ],
                                    "text": "Observe the changes in the output"
                                }
                            },
                            {
                                "box": {
                                    "bgmode": 0,
                                    "border": 1,
                                    "clickthrough": 0,
                                    "embed": 1,
                                    "enablehscroll": 0,
                                    "enablevscroll": 0,
                                    "id": "obj-1",
                                    "lockeddragscroll": 0,
                                    "maxclass": "bpatcher",
                                    "numinlets": 0,
                                    "numoutlets": 1,
                                    "offset": [
                                        0.0,
                                        0.0
                                    ],
                                    "outlettype": [
                                        ""
                                    ],
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
                                            747.0,
                                            174.0,
                                            357.0,
                                            480.0
                                        ],
                                        "bglocked": 0,
                                        "openinpresentation": 1,
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
                                        "assistshowspatchername": 0,
                                        "boxes": [
                                            {
                                                "box": {
                                                    "id": "obj-4",
                                                    "maxclass": "newobj",
                                                    "numinlets": 2,
                                                    "numoutlets": 1,
                                                    "outlettype": [
                                                        "int"
                                                    ],
                                                    "patching_rect": [
                                                        12.0,
                                                        203.0,
                                                        29.5,
                                                        22.0
                                                    ],
                                                    "text": "+ 1"
                                                }
                                            },
                                            {
                                                "box": {
                                                    "comment": "",
                                                    "id": "obj-11",
                                                    "index": 1,
                                                    "maxclass": "outlet",
                                                    "numinlets": 1,
                                                    "numoutlets": 0,
                                                    "patching_rect": [
                                                        12.0,
                                                        289.0,
                                                        30.0,
                                                        30.0
                                                    ]
                                                }
                                            },
                                            {
                                                "box": {
                                                    "id": "obj-8",
                                                    "maxclass": "message",
                                                    "numinlets": 2,
                                                    "numoutlets": 1,
                                                    "outlettype": [
                                                        ""
                                                    ],
                                                    "patching_rect": [
                                                        12.0,
                                                        245.0,
                                                        51.0,
                                                        22.0
                                                    ],
                                                    "text": "input $1"
                                                }
                                            },
                                            {
                                                "box": {
                                                    "fontsize": 12.0,
                                                    "id": "obj-1",
                                                    "maxclass": "live.tab",
                                                    "num_lines_patching": 4,
                                                    "num_lines_presentation": 4,
                                                    "numinlets": 1,
                                                    "numoutlets": 3,
                                                    "outlettype": [
                                                        "",
                                                        "",
                                                        "float"
                                                    ],
                                                    "parameter_enable": 1,
                                                    "patching_rect": [
                                                        12.0,
                                                        14.0,
                                                        116.0,
                                                        171.0
                                                    ],
                                                    "presentation": 1,
                                                    "presentation_rect": [
                                                        14.0,
                                                        14.0,
                                                        124.0,
                                                        171.0
                                                    ],
                                                    "saved_attribute_attributes": {
                                                        "valueof": {
                                                            "parameter_enum": [
                                                                "wrap",
                                                                "shrink",
                                                                "pad_in",
                                                                "pad_out"
                                                            ],
                                                            "parameter_longname": "live.tab",
                                                            "parameter_mmax": 3,
                                                            "parameter_shortname": "live.tab",
                                                            "parameter_type": 2,
                                                            "parameter_unitstyle": 9
                                                        }
                                                    },
                                                    "varname": "live.tab"
                                                }
                                            }
                                        ],
                                        "lines": [
                                            {
                                                "patchline": {
                                                    "destination": [
                                                        "obj-4",
                                                        0
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
                                                        "obj-8",
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
                                                        "obj-11",
                                                        0
                                                    ],
                                                    "midpoints": [
                                                        21.5,
                                                        267.0,
                                                        21.5,
                                                        267.0
                                                    ],
                                                    "source": [
                                                        "obj-8",
                                                        0
                                                    ]
                                                }
                                            }
                                        ]
                                    },
                                    "patching_rect": [
                                        345.0,
                                        240.0,
                                        150.0,
                                        195.0
                                    ],
                                    "viewvisibility": 1
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-33",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        555.0,
                                        120.0,
                                        23.0
                                    ],
                                    "text": "-60. 29.9 -80."
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
                                        15.0,
                                        495.0,
                                        62.0,
                                        23.0
                                    ],
                                    "text": "fl.tomax~"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-30",
                                    "maxclass": "newobj",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        180.0,
                                        300.0,
                                        117.0,
                                        23.0
                                    ],
                                    "text": "fl.register~ 100 0.1"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-29",
                                    "maxclass": "newobj",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
                                    "patching_rect": [
                                        15.0,
                                        300.0,
                                        124.0,
                                        23.0
                                    ],
                                    "text": "fl.register~ 40 30 20"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-28",
                                    "maxclass": "newobj",
                                    "numinlets": 3,
                                    "numoutlets": 1,
                                    "outlettype": [
                                        ""
                                    ],
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
                                            162.0,
                                            218.0,
                                            845.0,
                                            301.0
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
                                        "assistshowspatchername": 0,
                                        "boxes": [
                                            {
                                                "box": {
                                                    "id": "obj-6",
                                                    "maxclass": "newobj",
                                                    "numinlets": 1,
                                                    "numoutlets": 1,
                                                    "outlettype": [
                                                        ""
                                                    ],
                                                    "patching_rect": [
                                                        715.0,
                                                        132.0,
                                                        116.0,
                                                        22.0
                                                    ],
                                                    "text": "fl.frommax~ params"
                                                }
                                            },
                                            {
                                                "box": {
                                                    "comment": "Mode Selector",
                                                    "id": "obj-5",
                                                    "index": 3,
                                                    "maxclass": "inlet",
                                                    "numinlets": 0,
                                                    "numoutlets": 1,
                                                    "outlettype": [
                                                        ""
                                                    ],
                                                    "patching_rect": [
                                                        715.0,
                                                        24.0,
                                                        30.0,
                                                        30.0
                                                    ]
                                                }
                                            },
                                            {
                                                "box": {
                                                    "comment": "Result",
                                                    "id": "obj-4",
                                                    "index": 1,
                                                    "maxclass": "outlet",
                                                    "numinlets": 1,
                                                    "numoutlets": 0,
                                                    "patching_rect": [
                                                        22.0,
                                                        249.0,
                                                        30.0,
                                                        30.0
                                                    ]
                                                }
                                            },
                                            {
                                                "box": {
                                                    "comment": "Right Frame",
                                                    "id": "obj-2",
                                                    "index": 2,
                                                    "maxclass": "inlet",
                                                    "numinlets": 0,
                                                    "numoutlets": 1,
                                                    "outlettype": [
                                                        ""
                                                    ],
                                                    "patching_rect": [
                                                        119.0,
                                                        24.0,
                                                        30.0,
                                                        30.0
                                                    ]
                                                }
                                            },
                                            {
                                                "box": {
                                                    "comment": "Left Frame",
                                                    "id": "obj-1",
                                                    "index": 1,
                                                    "maxclass": "inlet",
                                                    "numinlets": 0,
                                                    "numoutlets": 1,
                                                    "outlettype": [
                                                        ""
                                                    ],
                                                    "patching_rect": [
                                                        22.0,
                                                        24.0,
                                                        30.0,
                                                        30.0
                                                    ]
                                                }
                                            },
                                            {
                                                "box": {
                                                    "id": "obj-27",
                                                    "maxclass": "newobj",
                                                    "numinlets": 2,
                                                    "numoutlets": 1,
                                                    "outlettype": [
                                                        ""
                                                    ],
                                                    "patching_rect": [
                                                        541.0,
                                                        132.0,
                                                        134.0,
                                                        22.0
                                                    ],
                                                    "text": "fl.-~ /mismatch pad_out"
                                                }
                                            },
                                            {
                                                "box": {
                                                    "id": "obj-26",
                                                    "maxclass": "newobj",
                                                    "numinlets": 2,
                                                    "numoutlets": 1,
                                                    "outlettype": [
                                                        ""
                                                    ],
                                                    "patching_rect": [
                                                        368.0,
                                                        132.0,
                                                        126.0,
                                                        22.0
                                                    ],
                                                    "text": "fl.-~ /mismatch pad_in"
                                                }
                                            },
                                            {
                                                "box": {
                                                    "id": "obj-25",
                                                    "maxclass": "newobj",
                                                    "numinlets": 5,
                                                    "numoutlets": 1,
                                                    "outlettype": [
                                                        ""
                                                    ],
                                                    "patching_rect": [
                                                        22.0,
                                                        207.0,
                                                        711.0,
                                                        22.0
                                                    ],
                                                    "text": "fl.select~ 4"
                                                }
                                            },
                                            {
                                                "box": {
                                                    "id": "obj-24",
                                                    "maxclass": "newobj",
                                                    "numinlets": 2,
                                                    "numoutlets": 1,
                                                    "outlettype": [
                                                        ""
                                                    ],
                                                    "patching_rect": [
                                                        195.0,
                                                        132.0,
                                                        122.0,
                                                        22.0
                                                    ],
                                                    "text": "fl.-~ /mismatch shrink"
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
                                                        22.0,
                                                        132.0,
                                                        116.0,
                                                        22.0
                                                    ],
                                                    "text": "fl.-~ /mismatch wrap"
                                                }
                                            }
                                        ],
                                        "lines": [
                                            {
                                                "patchline": {
                                                    "destination": [
                                                        "obj-13",
                                                        0
                                                    ],
                                                    "order": 3,
                                                    "source": [
                                                        "obj-1",
                                                        0
                                                    ]
                                                }
                                            },
                                            {
                                                "patchline": {
                                                    "destination": [
                                                        "obj-24",
                                                        0
                                                    ],
                                                    "midpoints": [
                                                        31.5,
                                                        118.0,
                                                        204.5,
                                                        118.0
                                                    ],
                                                    "order": 2,
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
                                                    "midpoints": [
                                                        31.5,
                                                        118.0,
                                                        377.5,
                                                        118.0
                                                    ],
                                                    "order": 1,
                                                    "source": [
                                                        "obj-1",
                                                        0
                                                    ]
                                                }
                                            },
                                            {
                                                "patchline": {
                                                    "destination": [
                                                        "obj-27",
                                                        0
                                                    ],
                                                    "midpoints": [
                                                        31.5,
                                                        118.0,
                                                        550.5,
                                                        118.0
                                                    ],
                                                    "order": 0,
                                                    "source": [
                                                        "obj-1",
                                                        0
                                                    ]
                                                }
                                            },
                                            {
                                                "patchline": {
                                                    "destination": [
                                                        "obj-25",
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
                                                    "color": [
                                                        0.0,
                                                        0.871366560459137,
                                                        0.0,
                                                        1.0
                                                    ],
                                                    "destination": [
                                                        "obj-13",
                                                        1
                                                    ],
                                                    "order": 4,
                                                    "source": [
                                                        "obj-2",
                                                        0
                                                    ]
                                                }
                                            },
                                            {
                                                "patchline": {
                                                    "color": [
                                                        0.0,
                                                        0.871366560459137,
                                                        0.0,
                                                        1.0
                                                    ],
                                                    "destination": [
                                                        "obj-24",
                                                        1
                                                    ],
                                                    "midpoints": [
                                                        128.5,
                                                        92.5,
                                                        307.5,
                                                        92.5
                                                    ],
                                                    "order": 3,
                                                    "source": [
                                                        "obj-2",
                                                        0
                                                    ]
                                                }
                                            },
                                            {
                                                "patchline": {
                                                    "color": [
                                                        0.0,
                                                        0.871366560459137,
                                                        0.0,
                                                        1.0
                                                    ],
                                                    "destination": [
                                                        "obj-26",
                                                        1
                                                    ],
                                                    "midpoints": [
                                                        128.5,
                                                        92.5,
                                                        484.5,
                                                        92.5
                                                    ],
                                                    "order": 2,
                                                    "source": [
                                                        "obj-2",
                                                        0
                                                    ]
                                                }
                                            },
                                            {
                                                "patchline": {
                                                    "color": [
                                                        0.0,
                                                        0.871366560459137,
                                                        0.0,
                                                        1.0
                                                    ],
                                                    "destination": [
                                                        "obj-27",
                                                        1
                                                    ],
                                                    "midpoints": [
                                                        128.5,
                                                        92.5,
                                                        665.5,
                                                        92.5
                                                    ],
                                                    "order": 1,
                                                    "source": [
                                                        "obj-2",
                                                        0
                                                    ]
                                                }
                                            },
                                            {
                                                "patchline": {
                                                    "color": [
                                                        0.0,
                                                        0.871366560459137,
                                                        0.0,
                                                        1.0
                                                    ],
                                                    "destination": [
                                                        "obj-6",
                                                        0
                                                    ],
                                                    "midpoints": [
                                                        128.5,
                                                        92.5,
                                                        724.5,
                                                        92.5
                                                    ],
                                                    "order": 0,
                                                    "source": [
                                                        "obj-2",
                                                        0
                                                    ]
                                                }
                                            },
                                            {
                                                "patchline": {
                                                    "destination": [
                                                        "obj-25",
                                                        1
                                                    ],
                                                    "source": [
                                                        "obj-24",
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
                                                        "obj-25",
                                                        0
                                                    ]
                                                }
                                            },
                                            {
                                                "patchline": {
                                                    "destination": [
                                                        "obj-25",
                                                        2
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
                                                        "obj-25",
                                                        3
                                                    ],
                                                    "source": [
                                                        "obj-27",
                                                        0
                                                    ]
                                                }
                                            },
                                            {
                                                "patchline": {
                                                    "destination": [
                                                        "obj-6",
                                                        0
                                                    ],
                                                    "source": [
                                                        "obj-5",
                                                        0
                                                    ]
                                                }
                                            },
                                            {
                                                "patchline": {
                                                    "destination": [
                                                        "obj-25",
                                                        4
                                                    ],
                                                    "source": [
                                                        "obj-6",
                                                        0
                                                    ]
                                                }
                                            }
                                        ]
                                    },
                                    "patching_rect": [
                                        15.0,
                                        450.0,
                                        349.0,
                                        23.0
                                    ],
                                    "saved_object_attributes": {
                                        "description": "",
                                        "digest": "",
                                        "globalpatchername": "",
                                        "tags": ""
                                    },
                                    "text": "p binary_mismatch"
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato",
                                    "id": "obj-2",
                                    "linecount": 9,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [
                                        15.0,
                                        90.0,
                                        615.0,
                                        147.0
                                    ],
                                    "text": "FrameLib objects have a configurable logic for dealing with scenarios where there is a mismatch between the lengths of different inputs (or pairs of inputs for objects dealing with complex numbers). If an object needs to deal with this problem it will possess a /mismatch parameter.\n\nThere are a number of modes for dealing with mismatched frame sizes and not all objects have the same (this will depend on whether they represent a binary or ternary operator, or something else).\n\nBelow is a generic demonstration of the four modes offered on all binary operators for mismatched inputs and the effect this has on the output."
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
                                        15.0,
                                        240.0,
                                        101.0,
                                        23.0
                                    ],
                                    "text": "fl.interval~ 1024"
                                }
                            },
                            {
                                "box": {
                                    "border": 0,
                                    "filename": "fl.helpname.js",
                                    "id": "obj-3",
                                    "ignoreclick": 1,
                                    "jsarguments": [
                                        "fl.clip~"
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
                                        60.0
                                    ]
                                }
                            }
                        ],
                        "lines": [
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-28",
                                        2
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
                                        "obj-31",
                                        0
                                    ],
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
                                        1
                                    ],
                                    "source": [
                                        "obj-30",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-33",
                                        1
                                    ],
                                    "midpoints": [
                                        24.5,
                                        536.0,
                                        125.5,
                                        536.0
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
                                        "obj-29",
                                        0
                                    ],
                                    "order": 1,
                                    "source": [
                                        "obj-4",
                                        0
                                    ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [
                                        "obj-30",
                                        0
                                    ],
                                    "midpoints": [
                                        24.5,
                                        287.0,
                                        189.5,
                                        287.0
                                    ],
                                    "order": 0,
                                    "source": [
                                        "obj-4",
                                        0
                                    ]
                                }
                            }
                        ],
                        "styles": [
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
                                "name": "newobjYellow-1",
                                "default": {
                                    "fontsize": [
                                        12.059008
                                    ],
                                    "accentcolor": [
                                        0.82517,
                                        0.78181,
                                        0.059545,
                                        1.0
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
                            }
                        ]
                    },
                    "patching_rect": [
                        271.0,
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
                    "text": "p mismatch"
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