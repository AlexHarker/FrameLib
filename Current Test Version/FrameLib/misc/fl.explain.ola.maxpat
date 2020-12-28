{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 8,
			"minor" : 1,
			"revision" : 3,
			"architecture" : "x64",
			"modernui" : 1
		}
,
		"classnamespace" : "box",
		"rect" : [ 34.0, 79.0, 879.0, 662.0 ],
		"bglocked" : 0,
		"openinpresentation" : 1,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 2,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 2,
		"objectsnaponopen" : 1,
		"statusbarvisible" : 2,
		"toolbarvisible" : 1,
		"lefttoolbarpinned" : 0,
		"toptoolbarpinned" : 0,
		"righttoolbarpinned" : 0,
		"bottomtoolbarpinned" : 0,
		"toolbars_unpinned_last_save" : 0,
		"tallnewobj" : 0,
		"boxanimatetime" : 200,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"description" : "",
		"digest" : "",
		"tags" : "",
		"style" : "",
		"subpatcher_template" : "",
		"boxes" : [ 			{
				"box" : 				{
					"id" : "obj-151",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 27.0, 471.333331644535065, 465.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 15.583333343267441, 600.333333969116211, 464.416666656732559, 20.0 ],
					"text" : "All time with frames overlapped and added (compensated by multipling frames by 0.5)",
					"textcolor" : [ 0.996078431372549, 0.996078431372549, 0.996078431372549, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-152",
					"ignoreclick" : 1,
					"maxclass" : "multislider",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 27.0, 403.0, 519.0, 88.333331644535065 ],
					"presentation" : 1,
					"presentation_rect" : [ 15.583333343267441, 489.000002324581203, 459.583333343267441, 131.333331644535065 ],
					"size" : 6144,
					"slidercolor" : [ 0.772549019607843, 0.305882352941176, 0.772549019607843, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-150",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 3,
					"outlettype" : [ "", "", "bang" ],
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 1,
							"revision" : 3,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"classnamespace" : "box",
						"rect" : [ 59.0, 104.0, 640.0, 480.0 ],
						"bglocked" : 0,
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 1,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 1,
						"objectsnaponopen" : 1,
						"statusbarvisible" : 2,
						"toolbarvisible" : 1,
						"lefttoolbarpinned" : 0,
						"toptoolbarpinned" : 0,
						"righttoolbarpinned" : 0,
						"bottomtoolbarpinned" : 0,
						"toolbars_unpinned_last_save" : 0,
						"tallnewobj" : 0,
						"boxanimatetime" : 200,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"style" : "",
						"subpatcher_template" : "",
						"boxes" : [ 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-8",
									"index" : 3,
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 214.699994500000003, 312.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-2",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patcher" : 									{
										"fileversion" : 1,
										"appversion" : 										{
											"major" : 8,
											"minor" : 1,
											"revision" : 3,
											"architecture" : "x64",
											"modernui" : 1
										}
,
										"classnamespace" : "box",
										"rect" : [ 84.0, 129.0, 640.0, 480.0 ],
										"bglocked" : 0,
										"openinpresentation" : 0,
										"default_fontsize" : 12.0,
										"default_fontface" : 0,
										"default_fontname" : "Arial",
										"gridonopen" : 1,
										"gridsize" : [ 15.0, 15.0 ],
										"gridsnaponopen" : 1,
										"objectsnaponopen" : 1,
										"statusbarvisible" : 2,
										"toolbarvisible" : 1,
										"lefttoolbarpinned" : 0,
										"toptoolbarpinned" : 0,
										"righttoolbarpinned" : 0,
										"bottomtoolbarpinned" : 0,
										"toolbars_unpinned_last_save" : 0,
										"tallnewobj" : 0,
										"boxanimatetime" : 200,
										"enablehscroll" : 1,
										"enablevscroll" : 1,
										"devicewidth" : 0.0,
										"description" : "",
										"digest" : "",
										"tags" : "",
										"style" : "",
										"subpatcher_template" : "",
										"boxes" : [ 											{
												"box" : 												{
													"id" : "obj-1",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "float" ],
													"patching_rect" : [ 94.800013068508179, 404.0, 33.0, 22.0 ],
													"text" : "* 0.5"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-140",
													"maxclass" : "newobj",
													"numinlets" : 3,
													"numoutlets" : 1,
													"outlettype" : [ "float" ],
													"patching_rect" : [ 427.400003135204315, 328.000003397464752, 55.0, 22.0 ],
													"text" : "peek~ tri"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-139",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "int" ],
													"patching_rect" : [ 427.400003135204315, 275.566659957170486, 43.0, 22.0 ],
													"text" : "- 2048"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-138",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "float" ],
													"patching_rect" : [ 282.800003468990326, 328.000003397464752, 29.5, 22.0 ],
													"text" : "+ 0."
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-130",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "int" ],
													"patching_rect" : [ 348.200002789497375, 242.399994134902954, 43.0, 22.0 ],
													"text" : "- 2048"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-129",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 2,
													"outlettype" : [ "int", "int" ],
													"patching_rect" : [ 282.800002038478851, 216.000001728534698, 75.900000691413879, 22.0 ],
													"text" : "t i i"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-128",
													"maxclass" : "newobj",
													"numinlets" : 3,
													"numoutlets" : 1,
													"outlettype" : [ "float" ],
													"patching_rect" : [ 348.200002789497375, 275.566659957170486, 55.0, 22.0 ],
													"text" : "peek~ tri"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-126",
													"maxclass" : "newobj",
													"numinlets" : 3,
													"numoutlets" : 2,
													"outlettype" : [ "int", "int" ],
													"patching_rect" : [ 427.400003135204315, 208.399994134902954, 127.600000560283661, 22.0 ],
													"text" : "split 4096 6144"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-125",
													"maxclass" : "newobj",
													"numinlets" : 3,
													"numoutlets" : 2,
													"outlettype" : [ "int", "int" ],
													"patching_rect" : [ 282.800002038478851, 176.000001132488251, 163.600001096725464, 22.0 ],
													"text" : "split 2048 4096"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-124",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 2,
													"outlettype" : [ "", "" ],
													"patching_rect" : [ 94.800013068508179, 433.800004363059998, 81.0, 22.0 ],
													"text" : "zl 6144 group"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-117",
													"maxclass" : "newobj",
													"numinlets" : 3,
													"numoutlets" : 2,
													"outlettype" : [ "int", "int" ],
													"patching_rect" : [ 95.0, 139.200000584125519, 206.800002038478851, 22.0 ],
													"text" : "split 0 2048"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-114",
													"maxclass" : "newobj",
													"numinlets" : 3,
													"numoutlets" : 1,
													"outlettype" : [ "float" ],
													"patching_rect" : [ 282.800002038478851, 275.566659957170486, 60.0, 22.0 ],
													"text" : "peek~ sin"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-113",
													"maxclass" : "newobj",
													"numinlets" : 3,
													"numoutlets" : 1,
													"outlettype" : [ "float" ],
													"patching_rect" : [ 95.0, 279.53332531452179, 60.0, 22.0 ],
													"text" : "peek~ sin"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-112",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 3,
													"outlettype" : [ "bang", "bang", "int" ],
													"patching_rect" : [ 50.0, 100.0, 64.0, 22.0 ],
													"text" : "uzi 6144 0"
												}

											}
, 											{
												"box" : 												{
													"comment" : "",
													"id" : "obj-145",
													"index" : 1,
													"maxclass" : "inlet",
													"numinlets" : 0,
													"numoutlets" : 1,
													"outlettype" : [ "bang" ],
													"patching_rect" : [ 50.000026068508191, 40.000000134902962, 30.0, 30.0 ]
												}

											}
, 											{
												"box" : 												{
													"comment" : "",
													"id" : "obj-146",
													"index" : 1,
													"maxclass" : "outlet",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 94.800013068508179, 474.800018134902984, 30.0, 30.0 ]
												}

											}
 ],
										"lines" : [ 											{
												"patchline" : 												{
													"destination" : [ "obj-124", 0 ],
													"source" : [ "obj-1", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-1", 0 ],
													"midpoints" : [ 82.0, 378.399994134902954, 104.300013068508179, 378.399994134902954 ],
													"source" : [ "obj-112", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-117", 0 ],
													"source" : [ "obj-112", 2 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-1", 0 ],
													"midpoints" : [ 104.5, 303.399994134902954, 104.300013068508179, 303.399994134902954 ],
													"source" : [ "obj-113", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-138", 0 ],
													"source" : [ "obj-114", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-113", 0 ],
													"source" : [ "obj-117", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-125", 0 ],
													"source" : [ "obj-117", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-146", 0 ],
													"source" : [ "obj-124", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-126", 0 ],
													"source" : [ "obj-125", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-129", 0 ],
													"source" : [ "obj-125", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-139", 0 ],
													"source" : [ "obj-126", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-138", 1 ],
													"source" : [ "obj-128", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-114", 0 ],
													"source" : [ "obj-129", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-130", 0 ],
													"source" : [ "obj-129", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-128", 0 ],
													"source" : [ "obj-130", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-1", 0 ],
													"midpoints" : [ 292.300003468990326, 378.399994134902954, 104.300013068508179, 378.399994134902954 ],
													"source" : [ "obj-138", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-140", 0 ],
													"source" : [ "obj-139", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-1", 0 ],
													"midpoints" : [ 436.900003135204315, 378.399994134902954, 104.300013068508179, 378.399994134902954 ],
													"source" : [ "obj-140", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-112", 0 ],
													"source" : [ "obj-145", 0 ]
												}

											}
 ]
									}
,
									"patching_rect" : [ 50.0, 230.0, 48.0, 22.0 ],
									"saved_object_attributes" : 									{
										"description" : "",
										"digest" : "",
										"globalpatchername" : "",
										"tags" : ""
									}
,
									"text" : "p comp"
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-3",
									"index" : 1,
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 50.0, 312.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-1",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "bang", "bang" ],
									"patching_rect" : [ 50.0, 182.0, 32.0, 22.0 ],
									"text" : "t b b"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-148",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 3,
									"outlettype" : [ "bang", "bang", "bang" ],
									"patching_rect" : [ 50.0, 124.0, 348.399989000000005, 22.0 ],
									"text" : "t b b b"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-147",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patcher" : 									{
										"fileversion" : 1,
										"appversion" : 										{
											"major" : 8,
											"minor" : 1,
											"revision" : 3,
											"architecture" : "x64",
											"modernui" : 1
										}
,
										"classnamespace" : "box",
										"rect" : [ 0.0, 0.0, 640.0, 480.0 ],
										"bglocked" : 0,
										"openinpresentation" : 0,
										"default_fontsize" : 12.0,
										"default_fontface" : 0,
										"default_fontname" : "Arial",
										"gridonopen" : 1,
										"gridsize" : [ 15.0, 15.0 ],
										"gridsnaponopen" : 1,
										"objectsnaponopen" : 1,
										"statusbarvisible" : 2,
										"toolbarvisible" : 1,
										"lefttoolbarpinned" : 0,
										"toptoolbarpinned" : 0,
										"righttoolbarpinned" : 0,
										"bottomtoolbarpinned" : 0,
										"toolbars_unpinned_last_save" : 0,
										"tallnewobj" : 0,
										"boxanimatetime" : 200,
										"enablehscroll" : 1,
										"enablevscroll" : 1,
										"devicewidth" : 0.0,
										"description" : "",
										"digest" : "",
										"tags" : "",
										"style" : "",
										"subpatcher_template" : "",
										"boxes" : [ 											{
												"box" : 												{
													"id" : "obj-140",
													"maxclass" : "newobj",
													"numinlets" : 3,
													"numoutlets" : 1,
													"outlettype" : [ "float" ],
													"patching_rect" : [ 427.400003135204315, 328.000003397464752, 55.0, 22.0 ],
													"text" : "peek~ tri"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-139",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "int" ],
													"patching_rect" : [ 427.400003135204315, 275.566659957170486, 43.0, 22.0 ],
													"text" : "- 2048"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-138",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "float" ],
													"patching_rect" : [ 282.800003468990326, 328.000003397464752, 29.5, 22.0 ],
													"text" : "+ 0."
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-130",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "int" ],
													"patching_rect" : [ 348.200002789497375, 242.399994134902954, 43.0, 22.0 ],
													"text" : "- 2048"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-129",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 2,
													"outlettype" : [ "int", "int" ],
													"patching_rect" : [ 282.800002038478851, 216.000001728534698, 75.900000691413879, 22.0 ],
													"text" : "t i i"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-128",
													"maxclass" : "newobj",
													"numinlets" : 3,
													"numoutlets" : 1,
													"outlettype" : [ "float" ],
													"patching_rect" : [ 348.200002789497375, 275.566659957170486, 55.0, 22.0 ],
													"text" : "peek~ tri"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-126",
													"maxclass" : "newobj",
													"numinlets" : 3,
													"numoutlets" : 2,
													"outlettype" : [ "int", "int" ],
													"patching_rect" : [ 427.400003135204315, 208.399994134902954, 127.600000560283661, 22.0 ],
													"text" : "split 4096 6144"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-125",
													"maxclass" : "newobj",
													"numinlets" : 3,
													"numoutlets" : 2,
													"outlettype" : [ "int", "int" ],
													"patching_rect" : [ 282.800002038478851, 176.000001132488251, 163.600001096725464, 22.0 ],
													"text" : "split 2048 4096"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-124",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 2,
													"outlettype" : [ "", "" ],
													"patching_rect" : [ 94.800000667572021, 392.800004363059998, 81.0, 22.0 ],
													"text" : "zl 6144 group"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-117",
													"maxclass" : "newobj",
													"numinlets" : 3,
													"numoutlets" : 2,
													"outlettype" : [ "int", "int" ],
													"patching_rect" : [ 95.0, 139.200000584125519, 206.800002038478851, 22.0 ],
													"text" : "split 0 2048"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-114",
													"maxclass" : "newobj",
													"numinlets" : 3,
													"numoutlets" : 1,
													"outlettype" : [ "float" ],
													"patching_rect" : [ 282.800002038478851, 275.566659957170486, 60.0, 22.0 ],
													"text" : "peek~ sin"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-113",
													"maxclass" : "newobj",
													"numinlets" : 3,
													"numoutlets" : 1,
													"outlettype" : [ "float" ],
													"patching_rect" : [ 95.0, 279.53332531452179, 60.0, 22.0 ],
													"text" : "peek~ sin"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-112",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 3,
													"outlettype" : [ "bang", "bang", "int" ],
													"patching_rect" : [ 50.0, 100.0, 64.0, 22.0 ],
													"text" : "uzi 6144 0"
												}

											}
, 											{
												"box" : 												{
													"comment" : "",
													"id" : "obj-145",
													"index" : 1,
													"maxclass" : "inlet",
													"numinlets" : 0,
													"numoutlets" : 1,
													"outlettype" : [ "bang" ],
													"patching_rect" : [ 50.000026068508191, 40.000000134902962, 30.0, 30.0 ]
												}

											}
, 											{
												"box" : 												{
													"comment" : "",
													"id" : "obj-146",
													"index" : 1,
													"maxclass" : "outlet",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 94.800013068508179, 474.800018134902984, 30.0, 30.0 ]
												}

											}
 ],
										"lines" : [ 											{
												"patchline" : 												{
													"destination" : [ "obj-117", 0 ],
													"source" : [ "obj-112", 2 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-124", 0 ],
													"midpoints" : [ 82.0, 378.399994134902954, 104.300000667572021, 378.399994134902954 ],
													"source" : [ "obj-112", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-124", 0 ],
													"midpoints" : [ 104.5, 303.399994134902954, 104.300000667572021, 303.399994134902954 ],
													"source" : [ "obj-113", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-138", 0 ],
													"source" : [ "obj-114", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-113", 0 ],
													"source" : [ "obj-117", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-125", 0 ],
													"source" : [ "obj-117", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-146", 0 ],
													"source" : [ "obj-124", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-126", 0 ],
													"source" : [ "obj-125", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-129", 0 ],
													"source" : [ "obj-125", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-139", 0 ],
													"source" : [ "obj-126", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-138", 1 ],
													"source" : [ "obj-128", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-114", 0 ],
													"source" : [ "obj-129", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-130", 0 ],
													"source" : [ "obj-129", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-128", 0 ],
													"source" : [ "obj-130", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-124", 0 ],
													"midpoints" : [ 292.300003468990326, 378.399994134902954, 104.300000667572021, 378.399994134902954 ],
													"source" : [ "obj-138", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-140", 0 ],
													"source" : [ "obj-139", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-124", 0 ],
													"midpoints" : [ 436.900003135204315, 378.399994134902954, 104.300000667572021, 378.399994134902954 ],
													"source" : [ "obj-140", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-112", 0 ],
													"source" : [ "obj-145", 0 ]
												}

											}
 ]
									}
,
									"patching_rect" : [ 100.0, 230.0, 68.0, 22.0 ],
									"saved_object_attributes" : 									{
										"description" : "",
										"digest" : "",
										"globalpatchername" : "",
										"tags" : ""
									}
,
									"text" : "p no_comp"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-68",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "bang" ],
									"patching_rect" : [ 50.0, 100.0, 58.0, 22.0 ],
									"text" : "loadbang"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-61",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 537.399989000000005, 192.0, 111.0, 22.0 ],
									"text" : "fill 1., apply triangle"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-62",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "float", "bang" ],
									"patching_rect" : [ 537.399989000000005, 230.0, 140.0, 22.0 ],
									"text" : "buffer~ tri @samps 4096"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-54",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 379.399989000000005, 192.0, 59.0, 22.0 ],
									"text" : "fill sin 2 1"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-52",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "float", "bang" ],
									"patching_rect" : [ 379.399989000000005, 230.0, 146.0, 22.0 ],
									"text" : "buffer~ sin @samps 4096"
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-149",
									"index" : 2,
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 100.0, 312.0, 30.0, 30.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-147", 0 ],
									"source" : [ "obj-1", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-2", 0 ],
									"source" : [ "obj-1", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-149", 0 ],
									"source" : [ "obj-147", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"source" : [ "obj-148", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-54", 0 ],
									"order" : 1,
									"source" : [ "obj-148", 2 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-61", 0 ],
									"order" : 0,
									"source" : [ "obj-148", 2 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-8", 0 ],
									"source" : [ "obj-148", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-3", 0 ],
									"source" : [ "obj-2", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-52", 0 ],
									"source" : [ "obj-54", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-62", 0 ],
									"source" : [ "obj-61", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-148", 0 ],
									"source" : [ "obj-68", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 27.0, 282.30000427365303, 95.0, 22.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p create_visuals"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"fontname" : "Lato Regular",
					"fontsize" : 12.0,
					"id" : "obj-141",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 144.375, 14.0, 252.0, 21.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 129.375, 6.666668355464935, 252.0, 21.0 ],
					"text" : "Overlapp-add frames demonstration",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-110",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 441.0, 61.0, 40.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 373.54629628525845, 53.666668355464935, 40.0, 20.0 ],
					"text" : "5120",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-109",
					"linecolor" : [ 0.137254901960784, 0.137254901960784, 0.137254901960784, 0.49 ],
					"maxclass" : "live.line",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 457.75, 82.0, 7.833333373069763, 115.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 387.740740713146067, 74.666668355464935, 11.611111144224765, 146.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-108",
					"linecolor" : [ 0.137254901960784, 0.137254901960784, 0.137254901960784, 0.49 ],
					"maxclass" : "live.line",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 542.416666656732559, 82.0, 7.166666686534882, 115.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 468.000000000000057, 74.666668355464935, 7.166666686534882, 146.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-107",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 259.5, 61.0, 54.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 213.694444427887618, 53.666668355464935, 54.0, 20.0 ],
					"text" : "3072",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-106",
					"linecolor" : [ 0.137254901960784, 0.137254901960784, 0.137254901960784, 0.49 ],
					"maxclass" : "live.line",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 283.25, 82.0, 6.5, 13.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 237.444444427887618, 74.666668355464935, 6.5, 13.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-105",
					"linecolor" : [ 0.137254901960784, 0.137254901960784, 0.137254901960784, 0.49 ],
					"maxclass" : "live.line",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 369.75, 82.0, 6.5, 13.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 312.592592570516842, 74.666668355464935, 6.5, 13.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-104",
					"linecolor" : [ 0.137254901960784, 0.137254901960784, 0.137254901960784, 0.49 ],
					"maxclass" : "live.line",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 196.75, 82.0, 6.5, 13.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 162.296296285258421, 74.666668355464935, 6.5, 13.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-103",
					"linecolor" : [ 0.137254901960784, 0.137254901960784, 0.137254901960784, 0.49 ],
					"maxclass" : "live.line",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 110.375, 82.0, 6.5, 13.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 87.148148142629211, 74.666668355464935, 6.5, 13.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-102",
					"linecolor" : [ 0.137254901960784, 0.137254901960784, 0.137254901960784, 0.49 ],
					"maxclass" : "live.line",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 24.0, 82.0, 6.5, 13.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 12.0, 74.666668355464935, 6.5, 13.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontface" : 2,
					"fontname" : "Lato Regular",
					"fontsize" : 12.0,
					"id" : "obj-100",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 144.375, 36.0, 245.0, 21.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 129.375, 28.666668355464935, 245.0, 21.0 ],
					"text" : "Time specified in samples",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-98",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 86.625, 61.0, 54.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 63.625, 53.666668355464935, 54.0, 20.0 ],
					"text" : "1024",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-96",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 1,
							"revision" : 3,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"classnamespace" : "box",
						"rect" : [ 59.0, 104.0, 640.0, 480.0 ],
						"bglocked" : 0,
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 1,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 1,
						"objectsnaponopen" : 1,
						"statusbarvisible" : 2,
						"toolbarvisible" : 1,
						"lefttoolbarpinned" : 0,
						"toptoolbarpinned" : 0,
						"righttoolbarpinned" : 0,
						"bottomtoolbarpinned" : 0,
						"toolbars_unpinned_last_save" : 0,
						"tallnewobj" : 0,
						"boxanimatetime" : 200,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"style" : "",
						"subpatcher_template" : "",
						"boxes" : [ 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-1",
									"index" : 1,
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "bang" ],
									"patching_rect" : [ 50.0, 75.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-64",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 3,
									"outlettype" : [ "bang", "bang", "int" ],
									"patching_rect" : [ 50.0, 131.0, 64.0, 22.0 ],
									"text" : "uzi 4096 0"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-65",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 95.0, 222.0, 81.0, 22.0 ],
									"text" : "zl 4096 group"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-66",
									"maxclass" : "newobj",
									"numinlets" : 3,
									"numoutlets" : 1,
									"outlettype" : [ "float" ],
									"patching_rect" : [ 95.0, 176.0, 55.0, 22.0 ],
									"text" : "peek~ tri"
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-95",
									"index" : 1,
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 95.0, 304.0, 30.0, 30.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-64", 0 ],
									"source" : [ "obj-1", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-65", 0 ],
									"source" : [ "obj-64", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-66", 0 ],
									"source" : [ "obj-64", 2 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-95", 0 ],
									"source" : [ "obj-65", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-65", 0 ],
									"source" : [ "obj-66", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 166.0, 199.0, 29.0, 22.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p tri"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-94",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 1,
							"revision" : 3,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"classnamespace" : "box",
						"rect" : [ 59.0, 104.0, 640.0, 480.0 ],
						"bglocked" : 0,
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 1,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 1,
						"objectsnaponopen" : 1,
						"statusbarvisible" : 2,
						"toolbarvisible" : 1,
						"lefttoolbarpinned" : 0,
						"toptoolbarpinned" : 0,
						"righttoolbarpinned" : 0,
						"bottomtoolbarpinned" : 0,
						"toolbars_unpinned_last_save" : 0,
						"tallnewobj" : 0,
						"boxanimatetime" : 200,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"style" : "",
						"subpatcher_template" : "",
						"boxes" : [ 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-1",
									"index" : 1,
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "bang" ],
									"patching_rect" : [ 50.0, 86.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-58",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 3,
									"outlettype" : [ "bang", "bang", "int" ],
									"patching_rect" : [ 50.0, 131.0, 64.0, 22.0 ],
									"text" : "uzi 4096 0"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-57",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 95.0, 222.0, 81.0, 22.0 ],
									"text" : "zl 4096 group"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-56",
									"maxclass" : "newobj",
									"numinlets" : 3,
									"numoutlets" : 1,
									"outlettype" : [ "float" ],
									"patching_rect" : [ 95.0, 176.0, 60.0, 22.0 ],
									"text" : "peek~ sin"
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-93",
									"index" : 1,
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 95.0, 304.0, 30.0, 30.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-58", 0 ],
									"source" : [ "obj-1", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-57", 0 ],
									"source" : [ "obj-56", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-93", 0 ],
									"source" : [ "obj-57", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-56", 0 ],
									"source" : [ "obj-58", 2 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-57", 0 ],
									"source" : [ "obj-58", 1 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ -9.0, 98.0, 34.0, 22.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p sin"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-84",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 27.0, 378.333331644535065, 386.666669607162476, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 15.583333343267441, 462.000002324581203, 386.666669607162476, 20.0 ],
					"text" : "All time with frames overlapped and added (no compensation)",
					"textcolor" : [ 0.996078431372549, 0.996078431372549, 0.996078431372549, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-83",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 200.0, 278.0, 92.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 162.916668146848707, 329.666669368743896, 92.0, 20.0 ],
					"text" : "Second Frame",
					"textcolor" : [ 0.996078431372549, 0.996078431372549, 0.996078431372549, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-82",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 27.0, 177.0, 76.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 12.0, 193.00000274181366, 76.0, 20.0 ],
					"text" : "First Frame",
					"textcolor" : [ 0.996078431372549, 0.996078431372549, 0.996078431372549, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-79",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 526.0, 61.0, 40.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 451.583333343267441, 53.666668355464935, 40.0, 20.0 ],
					"text" : "6144",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-78",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 349.0, 61.0, 48.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 291.842592570516899, 53.666668355464935, 48.0, 20.0 ],
					"text" : "4096",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-77",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 173.0, 61.0, 54.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 138.54629628525845, 53.666668355464935, 54.0, 20.0 ],
					"text" : "2048",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-76",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 18.25, 61.0, 18.0, 20.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 6.25, 53.666668355464935, 18.0, 20.0 ],
					"text" : "0",
					"textjustification" : 1
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-71",
					"ignoreclick" : 1,
					"maxclass" : "multislider",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 27.0, 310.0, 519.0, 88.333331644535065 ],
					"presentation" : 1,
					"presentation_rect" : [ 15.583333343267441, 353.666669368743896, 459.583333343267441, 132.33333295583725 ],
					"size" : 6144,
					"slidercolor" : [ 0.772549019607843, 0.305882352941176, 0.772549019607843, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-67",
					"ignoreclick" : 1,
					"maxclass" : "multislider",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 200.0, 199.0, 346.0, 99.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 162.296296285258421, 214.695997608815418, 312.70370371474155, 136.941342506577968 ],
					"size" : 4096,
					"slidercolor" : [ 0.827450980392157, 0.203921568627451, 0.270588235294118, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-51",
					"ignoreclick" : 1,
					"maxclass" : "multislider",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 27.0, 98.0, 346.0, 99.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 12.0, 90.666668355464935, 307.092592570516842, 122.333334386348724 ],
					"size" : 4096,
					"slidercolor" : [ 0.376470588235294, 0.925490196078431, 0.996078431372549, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Lato Regular",
					"fontsize" : 13.0,
					"id" : "obj-4",
					"linecount" : 30,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 572.399989000000005, 61.0, 416.0, 474.0 ],
					"presentation" : 1,
					"presentation_linecount" : 35,
					"presentation_rect" : [ 495.0, 74.666668355464935, 345.0, 552.0 ],
					"text" : "We can think of time in the audio domain as something that passes in numbers of samples. If our sampling rate is 44.1khz then 44100 samples pass each second. Here we have two imaginary frames both 4096 samples long. Lets imagine that the frame on the left was created first, at a time called '0 samples'. This frame is 4096 samples long and will last for this amount of time once it reaches the fl.sink~ overlap add buffer.\n\nLets imagine the object on the right is the same length, 4096 samples, but is created after 2048 samples of time have passed. This would happen exactly halfway through the duration of the first frame, and so the last 2048 samples of frame 1, would be added to the first 2048 samples of frame 2. We can describe what is happening here as the 'overlap', or more specifically a '2 times overlap'. We can mathematically calculate the overlap by dividing our frame size by the rate at which frames are being created. In this case: \n\n4096 (length) / 2048 (rate) = 2 (overlap)\n\n\nAs you can see in the output with no compensation, adding these frames results in clipping and the top of the sine wave component is squared off. When we properly compensate for the 2 times overlap by multiplying the inputs by 0.5 (half) the result is kept within a range which doesn't clip.\n\nThis is how frames arriving at fl.sink~ are processed simultaneously. If this isn't making complete sense yet, revisit some of the initial examples, especially tutorial 2, and observe how overlaps are created and compensated for inside the sub patchers."
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-152", 0 ],
					"source" : [ "obj-150", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-71", 0 ],
					"source" : [ "obj-150", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-94", 0 ],
					"midpoints" : [ 112.5, 306.0, 123.0, 306.0, 123.0, 207.0, 0.0, 207.0, 0.0, 120.0, -3.0, 120.0, -3.0, 93.0, 0.5, 93.0 ],
					"order" : 1,
					"source" : [ "obj-150", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-96", 0 ],
					"midpoints" : [ 112.5, 306.0, 132.0, 306.0, 132.0, 207.0, 162.0, 207.0, 162.0, 198.0, 175.5, 198.0 ],
					"order" : 0,
					"source" : [ "obj-150", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-51", 0 ],
					"source" : [ "obj-94", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-67", 0 ],
					"source" : [ "obj-96", 0 ]
				}

			}
 ]
	}

}
