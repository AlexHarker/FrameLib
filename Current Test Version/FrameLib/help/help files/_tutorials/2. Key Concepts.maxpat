{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 7,
			"minor" : 3,
			"revision" : 4,
			"architecture" : "x64",
			"modernui" : 1
		}
,
		"rect" : [ 69.0, 246.0, 667.0, 899.0 ],
		"bglocked" : 0,
		"openinpresentation" : 0,
		"default_fontsize" : 11.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 1,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 1,
		"objectsnaponopen" : 1,
		"statusbarvisible" : 2,
		"toolbarvisible" : 0,
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
		"subpatcher_template" : "default",
		"showrootpatcherontab" : 0,
		"showontab" : 0,
		"boxes" : [ 			{
				"box" : 				{
					"id" : "obj-20",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 0,
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 7,
							"minor" : 3,
							"revision" : 4,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"rect" : [ 0.0, 26.0, 667.0, 873.0 ],
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
						"subpatcher_template" : "default",
						"showontab" : 1,
						"title" : "Parameters and Tagging",
						"boxes" : [ 							{
								"box" : 								{
									"id" : "obj-21",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 112.0, 813.0, 47.75, 20.0 ],
									"style" : "",
									"text" : "Length"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-19",
									"maxclass" : "number",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "", "bang" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 59.0, 812.0, 50.0, 22.0 ],
									"style" : ""
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-15",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 15.75, 812.0, 39.0, 22.0 ],
									"style" : "",
									"text" : "zl len"
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-14",
									"linecount" : 11,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 8.75, 350.0, 608.0, 179.0 ],
									"style" : "",
									"text" : "3. fl.tag~ (real-time configuration.)\n\nReal-time control of parameters is managed uniquely in FrameLib. Each parameter does not have its own inlet like most Max objects do but works more closely to gen~ by sending in tagged messages with the appropriate parameter. These tagged frames designed for changing parameters in real-time are created with the fl.tag~ object and sent to the \"parameter update\" inlet. In the example below a new frame of random numbers between 0 and 1 are generated every 200 milliseconds, however, each time this happens the length of the frame (or how many numbers are generated) is also randomly decided (between 1-21). The multislider depicts the output and you can see how it changes each time.\n\n"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-13",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 8.75, 305.0, 297.0, 22.0 ],
									"style" : "",
									"text" : "fl.random~ /mode requested /length 10 /units samples"
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-12",
									"linecount" : 4,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 8.75, 234.0, 608.0, 69.0 ],
									"style" : "",
									"text" : "2. /parameter <value>\n\nParameters can also be addressed by prefxiing / to the name. Again, lets configure the same example from above in this style."
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-11",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 201.0, 203.0, 28.0, 20.0 ],
									"style" : "",
									"text" : "==",
									"textjustification" : 1
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-9",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 234.75, 202.0, 104.0, 22.0 ],
									"style" : "",
									"text" : "fl.random~ 0 10 0"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-5",
									"maxclass" : "ezadc~",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "signal", "signal" ],
									"patching_rect" : [ 179.0, 753.400024, 51.0, 51.0 ],
									"style" : ""
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-2",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 8.75, 202.0, 189.0, 22.0 ],
									"style" : "",
									"text" : "fl.random~ requested 10 samples"
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-18",
									"linecount" : 9,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 8.75, 50.0, 608.0, 147.0 ],
									"style" : "",
									"text" : "There three ways of dealing with parameters in FrameLib, one of which is analogous to the Max style of configuring objects.\n\n1. \"Max style configuring\"\n\nfl.random~ has three parameters: Mode, Length and Units. We can address each one of these parameters by sequentially addressing each one. Enumerated parameters where there are limited options can also be addressed through numbers counting from 0. The example below demonstrates two ways of initialising the three parameters of fl.random~ in a max style."
								}

							}
, 							{
								"box" : 								{
									"fontface" : 1,
									"fontsize" : 20.0,
									"id" : "obj-1",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 8.75, 12.0, 456.0, 29.0 ],
									"style" : "",
									"text" : "Parameters in FrameLib"
								}

							}
, 							{
								"box" : 								{
									"border" : 2.0,
									"id" : "obj-49",
									"linecolor" : [ 0.65098, 0.666667, 0.662745, 1.0 ],
									"maxclass" : "live.line",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 8.75, 43.0, 530.25, 5.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-55",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patcher" : 									{
										"fileversion" : 1,
										"appversion" : 										{
											"major" : 7,
											"minor" : 3,
											"revision" : 4,
											"architecture" : "x64",
											"modernui" : 1
										}
,
										"rect" : [ 59.0, 103.0, 640.0, 480.0 ],
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
										"subpatcher_template" : "default",
										"boxes" : [ 											{
												"box" : 												{
													"arrows" : 1,
													"border" : 4.0,
													"id" : "obj-7",
													"justification" : 1,
													"maxclass" : "live.line",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 140.0, 270.400024, 48.0, 14.0 ]
												}

											}
, 											{
												"box" : 												{
													"fontsize" : 14.0,
													"id" : "obj-8",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 190.0, 266.400024, 229.0, 22.0 ],
													"style" : "",
													"text" : "Tag with length parameter "
												}

											}
, 											{
												"box" : 												{
													"arrows" : 1,
													"border" : 4.0,
													"id" : "obj-5",
													"justification" : 1,
													"maxclass" : "live.line",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 121.0, 227.0, 48.0, 14.0 ]
												}

											}
, 											{
												"box" : 												{
													"fontsize" : 14.0,
													"id" : "obj-6",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 171.0, 223.0, 229.0, 22.0 ],
													"style" : "",
													"text" : "Round to nearest integer"
												}

											}
, 											{
												"box" : 												{
													"arrows" : 1,
													"border" : 4.0,
													"id" : "obj-3",
													"justification" : 1,
													"maxclass" : "live.line",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 121.0, 186.0, 48.0, 14.0 ]
												}

											}
, 											{
												"box" : 												{
													"fontsize" : 14.0,
													"id" : "obj-4",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 171.0, 182.0, 229.0, 22.0 ],
													"style" : "",
													"text" : "Make sure no zero length frames"
												}

											}
, 											{
												"box" : 												{
													"arrows" : 1,
													"border" : 4.0,
													"id" : "obj-1",
													"justification" : 1,
													"maxclass" : "live.line",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 129.0, 142.0, 48.0, 14.0 ]
												}

											}
, 											{
												"box" : 												{
													"fontsize" : 14.0,
													"id" : "obj-2",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 179.0, 138.0, 229.0, 22.0 ],
													"style" : "",
													"text" : "Scale between 0 and 20"
												}

											}
, 											{
												"box" : 												{
													"arrows" : 1,
													"border" : 4.0,
													"id" : "obj-30",
													"justification" : 1,
													"maxclass" : "live.line",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 129.0, 104.0, 48.0, 14.0 ]
												}

											}
, 											{
												"box" : 												{
													"fontsize" : 14.0,
													"id" : "obj-31",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 179.0, 100.0, 229.0, 22.0 ],
													"style" : "",
													"text" : "Random numbers between 0 and 1"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-51",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 50.0, 182.0, 69.0, 22.0 ],
													"style" : "",
													"text" : "fl.plus~ 1"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-46",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 50.0, 223.0, 68.0, 22.0 ],
													"style" : "",
													"text" : "fl.round~"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-45",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 50.0, 138.0, 81.0, 22.0 ],
													"style" : "",
													"text" : "fl.times~ 20"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-44",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 50.0, 100.0, 77.0, 22.0 ],
													"style" : "",
													"text" : "fl.random~"
												}

											}
, 											{
												"box" : 												{
													"id" : "obj-40",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 50.0, 266.400024, 88.0, 22.0 ],
													"style" : "",
													"text" : "fl.tag~ length"
												}

											}
, 											{
												"box" : 												{
													"comment" : "",
													"id" : "obj-53",
													"index" : 1,
													"maxclass" : "inlet",
													"numinlets" : 0,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 50.0, 40.0, 30.0, 30.0 ],
													"style" : ""
												}

											}
, 											{
												"box" : 												{
													"comment" : "",
													"id" : "obj-54",
													"index" : 1,
													"maxclass" : "outlet",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 50.0, 304.400024, 30.0, 30.0 ],
													"style" : ""
												}

											}
 ],
										"lines" : [ 											{
												"patchline" : 												{
													"destination" : [ "obj-54", 0 ],
													"source" : [ "obj-40", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-45", 0 ],
													"source" : [ "obj-44", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-51", 0 ],
													"source" : [ "obj-45", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-40", 0 ],
													"source" : [ "obj-46", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-46", 0 ],
													"source" : [ "obj-51", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-44", 0 ],
													"source" : [ "obj-53", 0 ]
												}

											}
 ]
									}
,
									"patching_rect" : [ 63.75, 616.0, 99.0, 22.0 ],
									"saved_object_attributes" : 									{
										"description" : "",
										"digest" : "",
										"globalpatchername" : "",
										"style" : "",
										"tags" : ""
									}
,
									"style" : "",
									"text" : "p random_length"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-52",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 15.75, 714.0, 59.0, 22.0 ],
									"style" : "",
									"text" : "fl.tomax~"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-39",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 15.75, 665.0, 67.0, 22.0 ],
									"style" : "",
									"text" : "fl.random~"
								}

							}
, 							{
								"box" : 								{
									"arrows" : 1,
									"border" : 4.0,
									"id" : "obj-30",
									"maxclass" : "live.line",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 163.0, 620.0, 48.0, 14.0 ]
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-31",
									"linecount" : 5,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 213.0, 584.5, 212.0, 85.0 ],
									"style" : "",
									"text" : "This abstraction generates a random number between 1-21 and is passed to \"fl.tag~ length\". This modifies the output length of the fl.random~ object below."
								}

							}
, 							{
								"box" : 								{
									"arrows" : 1,
									"border" : 4.0,
									"id" : "obj-32",
									"maxclass" : "live.line",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 123.75, 545.0, 36.0, 14.0 ]
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-33",
									"linecount" : 2,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 163.0, 533.0, 285.0, 38.0 ],
									"style" : "",
									"text" : "fl.interval~ is a scheduler. Here it is outputting a trigger frame every 200 ms."
								}

							}
, 							{
								"box" : 								{
									"candycane" : 5,
									"ghostbar" : 50,
									"id" : "obj-34",
									"maxclass" : "multislider",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 15.75, 753.400024, 157.75, 51.0 ],
									"setminmax" : [ 0.0, 1.0 ],
									"size" : 10,
									"style" : ""
								}

							}
, 							{
								"box" : 								{
									"color" : [ 0.0, 0.0, 0.0, 1.0 ],
									"id" : "obj-37",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 15.75, 541.0, 106.0, 22.0 ],
									"style" : "",
									"text" : "fl.interval~ 200 ms"
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-19", 0 ],
									"source" : [ "obj-15", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-15", 0 ],
									"source" : [ "obj-34", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-39", 0 ],
									"order" : 1,
									"source" : [ "obj-37", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-55", 0 ],
									"order" : 0,
									"source" : [ "obj-37", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-52", 0 ],
									"source" : [ "obj-39", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-34", 0 ],
									"source" : [ "obj-52", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-39", 1 ],
									"source" : [ "obj-55", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 246.0, 582.0, 139.0, 21.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"style" : "",
						"tags" : ""
					}
,
					"style" : "",
					"text" : "p Parameters and Tagging"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-10",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 0,
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 7,
							"minor" : 3,
							"revision" : 4,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"rect" : [ 0.0, 26.0, 667.0, 873.0 ],
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
						"subpatcher_template" : "default",
						"showontab" : 1,
						"title" : "Schedulers and Timing",
						"boxes" : [ 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-20",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 80.125, 296.0, 371.0, 22.0 ],
									"style" : "",
									"text" : "Schedule one frame at the start of each Max audio vector."
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-10",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 8.75, 296.0, 72.0, 22.0 ],
									"style" : "",
									"text" : "fl.perblock~"
								}

							}
, 							{
								"box" : 								{
									"fontface" : 1,
									"fontsize" : 20.0,
									"id" : "obj-41",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 8.75, 348.0, 456.0, 29.0 ],
									"style" : "",
									"text" : "A basic example:"
								}

							}
, 							{
								"box" : 								{
									"border" : 2.0,
									"id" : "obj-42",
									"linecolor" : [ 0.65098, 0.666667, 0.662745, 1.0 ],
									"maxclass" : "live.line",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 8.75, 379.0, 530.25, 5.0 ]
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-40",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 68.75, 254.0, 342.0, 22.0 ],
									"style" : "",
									"text" : "Schedule trigger frames in the future at a given time."
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-39",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 59.75, 212.0, 257.0, 22.0 ],
									"style" : "",
									"text" : "Outputs the current time in samples."
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-38",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 100.75, 170.0, 257.0, 22.0 ],
									"style" : "",
									"text" : "Turns non-zero samples into triggers."
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-37",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 63.75, 128.0, 352.0, 22.0 ],
									"style" : "",
									"text" : "Outputs a single trigger frame at the beginning of time."
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-36",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 76.75, 86.0, 281.0, 22.0 ],
									"style" : "",
									"text" : "Outputs trigger frames at a repetitive rate."
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-35",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 8.75, 254.0, 58.0, 22.0 ],
									"style" : "",
									"text" : "fl.future~"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-34",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 8.75, 212.0, 49.0, 22.0 ],
									"style" : "",
									"text" : "fl.now~"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-33",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 8.75, 170.0, 90.0, 22.0 ],
									"style" : "",
									"text" : "fl.audiotrigger~"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-32",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 8.75, 128.0, 53.0, 22.0 ],
									"style" : "",
									"text" : "fl.once~"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-28",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 8.75, 86.0, 66.0, 22.0 ],
									"style" : "",
									"text" : "fl.interval~"
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-3",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 8.75, 52.0, 257.0, 22.0 ],
									"style" : "",
									"text" : "There are six schedulers in FrameLib:"
								}

							}
, 							{
								"box" : 								{
									"fontface" : 1,
									"fontsize" : 20.0,
									"id" : "obj-1",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 8.75, 12.0, 456.0, 29.0 ],
									"style" : "",
									"text" : "Schedulers in FrameLib"
								}

							}
, 							{
								"box" : 								{
									"border" : 2.0,
									"id" : "obj-49",
									"linecolor" : [ 0.65098, 0.666667, 0.662745, 1.0 ],
									"maxclass" : "live.line",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 8.75, 43.0, 530.25, 5.0 ]
								}

							}
, 							{
								"box" : 								{
									"arrows" : 2,
									"border" : 4.0,
									"id" : "obj-19",
									"justification" : 1,
									"maxclass" : "live.line",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 80.125, 575.0, 31.25, 49.0 ]
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-18",
									"linecount" : 4,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 39.950012, 504.0, 604.0, 69.0 ],
									"style" : "",
									"text" : "The frame travelling along the black patch cable containing 5 floating pointing numbers acts as a trigger frame for the fl.random~ object below. We can see that the object has \"/mode input\" as a parameter. This means that it will output a frame as long as the trigger frame that it receives (meaning it will be 5 samples long). Lots of objects work like this but we'll come to that later."
								}

							}
, 							{
								"box" : 								{
									"candycane" : 5,
									"ghostbar" : 50,
									"id" : "obj-17",
									"maxclass" : "multislider",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 174.950012, 701.400024, 157.75, 51.0 ],
									"setminmax" : [ 0.0, 1.0 ],
									"size" : 5,
									"style" : ""
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-16",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 174.950012, 674.0, 59.0, 22.0 ],
									"style" : "",
									"text" : "fl.tomax~"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-15",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 174.950012, 645.400024, 134.0, 22.0 ],
									"style" : "",
									"text" : "fl.random~ /mode input"
								}

							}
, 							{
								"box" : 								{
									"arrows" : 1,
									"border" : 4.0,
									"id" : "obj-14",
									"justification" : 1,
									"maxclass" : "live.line",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 126.75, 442.5, 53.125, 16.0 ]
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-13",
									"linecount" : 2,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 181.875, 434.5, 368.0, 38.0 ],
									"style" : "",
									"text" : "fl.random~ generates a frame of pseudorandom floats between 0 and 1 at a specified length (set by /length)."
								}

							}
, 							{
								"box" : 								{
									"arrows" : 1,
									"border" : 4.0,
									"id" : "obj-12",
									"maxclass" : "live.line",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 103.625, 395.0, 53.125, 16.0 ]
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-11",
									"linecount" : 2,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 158.75, 386.5, 306.0, 38.0 ],
									"style" : "",
									"text" : "fl.interval~ is a scheduler. Here it is outputting a trigger frame every 2048 samples."
								}

							}
, 							{
								"box" : 								{
									"candycane" : 5,
									"ghostbar" : 50,
									"id" : "obj-8",
									"maxclass" : "multislider",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 8.75, 701.400024, 157.75, 51.0 ],
									"setminmax" : [ 0.0, 1.0 ],
									"size" : 5,
									"style" : ""
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-7",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 8.75, 674.0, 59.0, 22.0 ],
									"style" : "",
									"text" : "fl.tomax~"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-5",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 8.75, 440.0, 116.0, 22.0 ],
									"style" : "",
									"text" : "fl.random~ /length 5"
								}

							}
, 							{
								"box" : 								{
									"color" : [ 0.0, 0.0, 0.0, 1.0 ],
									"id" : "obj-4",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 8.75, 391.0, 94.0, 22.0 ],
									"style" : "",
									"text" : "fl.interval~ 2048"
								}

							}
, 							{
								"box" : 								{
									"background" : 1,
									"bgcolor" : [ 0.720698, 0.16723, 0.080014, 1.0 ],
									"fontface" : 1,
									"hint" : "",
									"id" : "obj-27",
									"ignoreclick" : 1,
									"legacytextcolor" : 1,
									"maxclass" : "textbutton",
									"numinlets" : 1,
									"numoutlets" : 3,
									"outlettype" : [ "", "", "int" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 645.950012, 528.5, 20.0, 20.0 ],
									"presentation_rect" : [ 271.057159, 258.0, 20.455872, 22.94873 ],
									"rounded" : 60.0,
									"style" : "",
									"text" : "3",
									"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ]
								}

							}
, 							{
								"box" : 								{
									"background" : 1,
									"bgcolor" : [ 0.720698, 0.16723, 0.080014, 1.0 ],
									"fontface" : 1,
									"hint" : "",
									"id" : "obj-26",
									"ignoreclick" : 1,
									"legacytextcolor" : 1,
									"maxclass" : "textbutton",
									"numinlets" : 1,
									"numoutlets" : 3,
									"outlettype" : [ "", "", "int" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 523.950012, 443.5, 20.0, 20.0 ],
									"presentation_rect" : [ 232.857147, 187.5, 20.455872, 22.94873 ],
									"rounded" : 60.0,
									"style" : "",
									"text" : "2",
									"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ]
								}

							}
, 							{
								"box" : 								{
									"background" : 1,
									"bgcolor" : [ 0.720698, 0.16723, 0.080014, 1.0 ],
									"fontface" : 1,
									"hint" : "",
									"id" : "obj-30",
									"ignoreclick" : 1,
									"legacytextcolor" : 1,
									"maxclass" : "textbutton",
									"numinlets" : 1,
									"numoutlets" : 3,
									"outlettype" : [ "", "", "int" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 466.75, 395.0, 20.0, 20.0 ],
									"presentation_rect" : [ 217.857147, 172.5, 20.455872, 22.94873 ],
									"rounded" : 60.0,
									"style" : "",
									"text" : "1",
									"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-16", 0 ],
									"source" : [ "obj-15", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-17", 0 ],
									"source" : [ "obj-16", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-5", 0 ],
									"source" : [ "obj-4", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"color" : [ 0.0, 0.0, 0.0, 1.0 ],
									"destination" : [ "obj-15", 0 ],
									"midpoints" : [ 18.25, 630.0, 184.450012, 630.0 ],
									"order" : 0,
									"source" : [ "obj-5", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-7", 0 ],
									"midpoints" : [ 18.25, 465.0, 18.25, 465.0 ],
									"order" : 1,
									"source" : [ "obj-5", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-8", 0 ],
									"source" : [ "obj-7", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 111.0, 582.0, 132.0, 21.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"style" : "",
						"tags" : ""
					}
,
					"style" : "",
					"text" : "p Schedulers and Timing"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-2",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 0,
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 7,
							"minor" : 3,
							"revision" : 4,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"rect" : [ 69.0, 272.0, 667.0, 873.0 ],
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
						"subpatcher_template" : "default",
						"showontab" : 1,
						"title" : "Key Concepts",
						"boxes" : [ 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-34",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 222.0, 841.0, 173.0, 22.0 ],
									"style" : "",
									"text" : "Provides a quick summary"
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-33",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 252.0, 810.333313, 204.0, 22.0 ],
									"style" : "",
									"text" : "Prints parameters for the object"
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-32",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 204.0, 779.666687, 209.0, 22.0 ],
									"style" : "",
									"text" : "Prints info on inputs and outputs"
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-31",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 234.0, 749.0, 230.0, 22.0 ],
									"style" : "",
									"text" : "Prints information about the outputs"
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-30",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 222.0, 718.333313, 221.0, 22.0 ],
									"style" : "",
									"text" : "Prints information about the inputs"
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-29",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 252.0, 687.666687, 139.0, 22.0 ],
									"style" : "",
									"text" : "Prints the description"
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-28",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 191.0, 657.0, 233.0, 22.0 ],
									"style" : "",
									"text" : "Prints all information to max console"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-26",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 159.0, 840.0, 61.0, 22.0 ],
									"style" : "",
									"text" : "info quick"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-25",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 159.0, 809.333313, 94.0, 22.0 ],
									"style" : "",
									"text" : "info parameters"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-24",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 159.0, 778.666687, 43.0, 22.0 ],
									"style" : "",
									"text" : "info io"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-23",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 159.0, 748.0, 73.0, 22.0 ],
									"style" : "",
									"text" : "info outputs"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-22",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 159.0, 717.333313, 65.0, 22.0 ],
									"style" : "",
									"text" : "info inputs"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-21",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 159.0, 686.666687, 91.0, 22.0 ],
									"style" : "",
									"text" : "info description"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-20",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 159.0, 656.0, 30.0, 22.0 ],
									"style" : "",
									"text" : "info"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-15",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 3.75, 656.0, 68.0, 22.0 ],
									"style" : "",
									"text" : "fl.random~"
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-6",
									"linecount" : 2,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 3.75, 606.0, 684.0, 38.0 ],
									"style" : "",
									"text" : "Like all Max objects there are help files that can be accessed for reference. You can also send FrameLib objects some reserved messages which post extra information to the max console. "
								}

							}
, 							{
								"box" : 								{
									"fontface" : 1,
									"fontsize" : 20.0,
									"id" : "obj-7",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 3.75, 568.0, 456.0, 29.0 ],
									"style" : "",
									"text" : "3. Learning how an object works."
								}

							}
, 							{
								"box" : 								{
									"border" : 2.0,
									"id" : "obj-8",
									"linecolor" : [ 0.65098, 0.666667, 0.662745, 1.0 ],
									"maxclass" : "live.line",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 3.75, 599.0, 530.25, 5.0 ]
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-2",
									"linecount" : 11,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 3.75, 370.0, 686.0, 179.0 ],
									"style" : "",
									"text" : "FrameLib objects have a unique system for managing parameters at both instantiation and with real-time control. It is is not dissimilar to max messags but there are some differences.\n\nAt object instanatiation there are two ways to assign parameters. Like many max objects you can input numbers that correspond to the arguments you are intending to address. Additionally you can directly address parameters through the \"/parameter <value>\". This is useful if the default configuration of an object is useful apart from one specific parameter, or for returning to patches where the arguments would otherwise not be obvious.\n\nReal-time control is managed through a single \"parameter update\" inlet in combination with the fl.tag~ object.\n\n"
								}

							}
, 							{
								"box" : 								{
									"fontface" : 1,
									"fontsize" : 20.0,
									"id" : "obj-4",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 3.75, 332.0, 456.0, 29.0 ],
									"style" : "",
									"text" : "2. Parameters and Tagging"
								}

							}
, 							{
								"box" : 								{
									"border" : 2.0,
									"id" : "obj-5",
									"linecolor" : [ 0.65098, 0.666667, 0.662745, 1.0 ],
									"maxclass" : "live.line",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 3.75, 363.0, 530.25, 5.0 ]
								}

							}
, 							{
								"box" : 								{
									"fontsize" : 14.0,
									"id" : "obj-3",
									"linecount" : 14,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 3.75, 105.0, 680.0, 225.0 ],
									"style" : "",
									"text" : "The most important concepts to understand in FrameLib:\n\n1. EVERYTHING is strongly timed (accurate to 2 ^ -64 of a sample).\n2. NOTHING happens without a timing source.\n3. Objects that are timing sources are called schedulers.\n\nSchedulers create trigger frames - think of it like the bang of FrameLib. Under the hood, a trigger frame is just an empty frame. In many situations, a non-empty frame can be used as a trigger. For example, a frame containing random numbers might act as a trigger frame for another random number generator. in scenarious such as these, the object being triggerd can take information from these frames such as their length and shape their output in reference to this.\n\nIn order for your patch to do anything at all it will need at least one scheduler. \n\n"
								}

							}
, 							{
								"box" : 								{
									"fontface" : 1,
									"fontsize" : 20.0,
									"id" : "obj-1",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 3.75, 65.0, 456.0, 29.0 ],
									"style" : "",
									"text" : "1. Schedulers and Timing"
								}

							}
, 							{
								"box" : 								{
									"border" : 2.0,
									"id" : "obj-49",
									"linecolor" : [ 0.65098, 0.666667, 0.662745, 1.0 ],
									"maxclass" : "live.line",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 3.75, 96.0, 530.25, 5.0 ]
								}

							}
, 							{
								"box" : 								{
									"fontface" : 0,
									"fontname" : "Lato",
									"fontsize" : 44.0,
									"id" : "obj-43",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 3.75, 4.0, 666.0, 59.0 ],
									"style" : "",
									"text" : "Key Concepts"
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-15", 0 ],
									"midpoints" : [ 168.5, 681.0, 81.0, 681.0, 81.0, 651.0, 13.25, 651.0 ],
									"source" : [ "obj-20", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-15", 0 ],
									"midpoints" : [ 168.5, 711.0, 81.0, 711.0, 81.0, 651.0, 13.25, 651.0 ],
									"source" : [ "obj-21", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-15", 0 ],
									"midpoints" : [ 168.5, 741.0, 81.0, 741.0, 81.0, 651.0, 13.25, 651.0 ],
									"source" : [ "obj-22", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-15", 0 ],
									"midpoints" : [ 168.5, 771.0, 81.0, 771.0, 81.0, 651.0, 13.25, 651.0 ],
									"source" : [ "obj-23", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-15", 0 ],
									"midpoints" : [ 168.5, 801.0, 81.0, 801.0, 81.0, 651.0, 13.25, 651.0 ],
									"source" : [ "obj-24", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-15", 0 ],
									"midpoints" : [ 168.5, 834.0, 81.0, 834.0, 81.0, 651.0, 13.25, 651.0 ],
									"source" : [ "obj-25", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-15", 0 ],
									"midpoints" : [ 168.5, 864.0, 81.0, 864.0, 81.0, 651.0, 13.25, 651.0 ],
									"source" : [ "obj-26", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 21.0, 582.0, 88.0, 21.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"style" : "",
						"tags" : ""
					}
,
					"style" : "",
					"text" : "p Key Concepts"
				}

			}
 ],
		"lines" : [  ],
		"dependency_cache" : [ 			{
				"name" : "fl.random~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.interval~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.interval~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.tomax~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.once~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.once~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.audiotrigger~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.audiotrigger~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.now~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.future~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.future~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.perblock~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.perblock~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.tag~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.times~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.round~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.plus~.mxo",
				"type" : "iLaX"
			}
 ],
		"autosave" : 0,
		"bgfillcolor_type" : "gradient",
		"bgfillcolor_color1" : [ 0.376471, 0.384314, 0.4, 1.0 ],
		"bgfillcolor_color2" : [ 0.290196, 0.309804, 0.301961, 1.0 ],
		"bgfillcolor_color" : [ 0.290196, 0.309804, 0.301961, 1.0 ],
		"bgfillcolor_angle" : 270.0,
		"bgfillcolor_proportion" : 0.39
	}

}
