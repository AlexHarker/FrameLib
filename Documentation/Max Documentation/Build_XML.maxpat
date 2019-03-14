{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 8,
			"minor" : 0,
			"revision" : 2,
			"architecture" : "x64",
			"modernui" : 1
		}
,
		"classnamespace" : "box",
		"rect" : [ 0.0, 45.0, 1440.0, 855.0 ],
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
		"subpatcher_template" : "nobars",
		"boxes" : [ 			{
				"box" : 				{
					"id" : "obj-585",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 970.66668701171875, 677.00006103515625, 62.0, 22.0 ],
					"text" : "readagain"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-109",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 927.1666259765625, 746.3333740234375, 77.0, 22.0 ],
					"text" : "route symbol"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-108",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 927.1666259765625, 677.00006103515625, 39.0, 22.0 ],
					"text" : "dump"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-103",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 4,
					"outlettype" : [ "", "", "", "" ],
					"patching_rect" : [ 927.1666259765625, 711.666748046875, 50.5, 22.0 ],
					"saved_object_attributes" : 					{
						"embed" : 0
					}
,
					"text" : "coll"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-95",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 3,
					"outlettype" : [ "bang", "", "" ],
					"patching_rect" : [ 1107.0, 833.00006103515625, 40.0, 22.0 ],
					"text" : "t b l l"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-94",
					"maxclass" : "newobj",
					"numinlets" : 3,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 1107.0, 898.00006103515625, 39.99993896484375, 22.0 ],
					"text" : "join 3"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-92",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 0,
							"revision" : 2,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"classnamespace" : "box",
						"rect" : [ 59.0, 104.0, 640.0, 562.0 ],
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
						"subpatcher_template" : "nobars",
						"boxes" : [ 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-1",
									"index" : 1,
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 50.0, 487.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-85",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 50.0, 446.666656494140625, 47.0, 22.0 ],
									"text" : "pack f f"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-82",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "bang", "bang" ],
									"patching_rect" : [ 50.0, 100.0, 32.0, 22.0 ],
									"text" : "t b b"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-81",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "int" ],
									"patching_rect" : [ 176.99993896484375, 291.0, 32.0, 22.0 ],
									"text" : "+ 30"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-80",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "int" ],
									"patching_rect" : [ 50.1666259765625, 383.0, 37.0, 22.0 ],
									"text" : "* 120"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-79",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "int" ],
									"patching_rect" : [ 50.1666259765625, 411.000030517578125, 32.0, 22.0 ],
									"text" : "+ 30"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-76",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "int" ],
									"patching_rect" : [ 50.1666259765625, 355.0, 36.0, 22.0 ],
									"text" : "% 15"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-75",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "int" ],
									"patching_rect" : [ 50.1666259765625, 322.0, 29.5, 22.0 ],
									"text" : "i 0"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-74",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "bang", "" ],
									"patching_rect" : [ 60.6666259765625, 237.333343505859375, 34.0, 22.0 ],
									"text" : "sel 8"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-65",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "int", "int" ],
									"patching_rect" : [ 166.49993896484375, 234.666656494140625, 29.5, 22.0 ],
									"text" : "t i i"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-62",
									"maxclass" : "newobj",
									"numinlets" : 5,
									"numoutlets" : 4,
									"outlettype" : [ "int", "", "", "int" ],
									"patching_rect" : [ 60.6666259765625, 291.0, 61.0, 22.0 ],
									"text" : "counter"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-54",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "int", "int" ],
									"patching_rect" : [ 129.99993896484375, 173.666656494140625, 55.5, 22.0 ],
									"text" : "t i i"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-52",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "int" ],
									"patching_rect" : [ 176.99993896484375, 261.666656494140625, 30.0, 22.0 ],
									"text" : "* 30"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-36",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "int" ],
									"patching_rect" : [ 166.49993896484375, 206.666656494140625, 36.0, 22.0 ],
									"text" : "% 15"
								}

							}
, 							{
								"box" : 								{
									"id" : "obj-35",
									"maxclass" : "newobj",
									"numinlets" : 5,
									"numoutlets" : 4,
									"outlettype" : [ "int", "", "", "int" ],
									"patching_rect" : [ 129.99993896484375, 142.666656494140625, 61.0, 22.0 ],
									"text" : "counter"
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-90",
									"index" : 1,
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "bang" ],
									"patching_rect" : [ 50.0, 40.0, 30.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"comment" : "",
									"id" : "obj-91",
									"index" : 2,
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "bang" ],
									"patching_rect" : [ 89.33331298828125, 40.0, 30.0, 30.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-54", 0 ],
									"source" : [ "obj-35", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-65", 0 ],
									"source" : [ "obj-36", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-81", 0 ],
									"source" : [ "obj-52", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-36", 0 ],
									"source" : [ "obj-54", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-75", 1 ],
									"source" : [ "obj-62", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-52", 0 ],
									"source" : [ "obj-65", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-74", 0 ],
									"source" : [ "obj-65", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-62", 0 ],
									"source" : [ "obj-74", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-76", 0 ],
									"source" : [ "obj-75", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-80", 0 ],
									"source" : [ "obj-76", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-85", 0 ],
									"source" : [ "obj-79", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-79", 0 ],
									"source" : [ "obj-80", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-85", 1 ],
									"source" : [ "obj-81", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-35", 0 ],
									"source" : [ "obj-82", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-75", 0 ],
									"source" : [ "obj-82", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"source" : [ "obj-85", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-82", 0 ],
									"source" : [ "obj-90", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-35", 2 ],
									"order" : 0,
									"source" : [ "obj-91", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-62", 2 ],
									"order" : 1,
									"source" : [ "obj-91", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 1107.0, 863.3333740234375, 91.0, 22.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p position_logic"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-89",
					"maxclass" : "button",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 1179.0, 823.3333740234375, 24.0, 24.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-26",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 927.1666259765625, 893.00006103515625, 131.0, 22.0 ],
					"text" : "connect method 0 $1 0"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-24",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 8.333351135253906, 60.0, 22.0 ],
					"text" : "write_info",
					"varname" : "method"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-22",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 927.1666259765625, 960.00006103515625, 67.0, 22.0 ],
					"save" : [ "#N", "thispatcher", ";", "#Q", "end", ";" ],
					"text" : "thispatcher"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-21",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 927.1666259765625, 783.00006103515625, 197.5, 22.0 ],
					"text" : "t l l"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-20",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 927.1666259765625, 927.00006103515625, 85.0, 22.0 ],
					"text" : "prepend script"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-9",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 1107.0, 927.00006103515625, 89.0, 22.0 ],
					"text" : "prepend create"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-8",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 1107.0, 960.00006103515625, 58.0, 22.0 ],
					"saved_object_attributes" : 					{
						"filename" : "creator",
						"parameter_enable" : 0
					}
,
					"text" : "js creator"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-1",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 43.0, 100.0, 22.0 ],
					"text" : "fl.and~",
					"varname" : "fl.and~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-2",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 73.0, 100.0, 22.0 ],
					"text" : "fl.atan2~",
					"varname" : "fl.atan2~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-3",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 103.0, 100.0, 22.0 ],
					"text" : "fl.copysign~",
					"varname" : "fl.copysign~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-4",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 133.0, 100.0, 22.0 ],
					"text" : "fl.diff~",
					"varname" : "fl.diff~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-5",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 163.0, 100.0, 22.0 ],
					"text" : "fl.divide~",
					"varname" : "fl.divide~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-6",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 193.0, 100.0, 22.0 ],
					"text" : "fl.equals~",
					"varname" : "fl.equals~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-7",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 223.0, 100.0, 22.0 ],
					"text" : "fl.greaterthan~",
					"varname" : "fl.greaterthan~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-10",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 253.0, 100.0, 22.0 ],
					"text" : "fl.greaterthaneq~",
					"varname" : "fl.greaterthaneq~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-11",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 283.0, 100.0, 22.0 ],
					"text" : "fl.hypot~",
					"varname" : "fl.hypot~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-12",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 313.0, 100.0, 22.0 ],
					"text" : "fl.lessthan~",
					"varname" : "fl.lessthan~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-13",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 343.0, 100.0, 22.0 ],
					"text" : "fl.lessthaneq~",
					"varname" : "fl.lessthaneq~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-14",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 373.0, 100.0, 22.0 ],
					"text" : "fl.max~",
					"varname" : "fl.max~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-15",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 403.0, 100.0, 22.0 ],
					"text" : "fl.min~",
					"varname" : "fl.min~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-16",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 433.0, 100.0, 22.0 ],
					"text" : "fl.minus~",
					"varname" : "fl.minus~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-17",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 463.0, 100.0, 22.0 ],
					"text" : "fl.modulo~",
					"varname" : "fl.modulo~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-18",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 43.0, 100.0, 22.0 ],
					"text" : "fl.notequals~",
					"varname" : "fl.notequals~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-19",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 73.0, 100.0, 22.0 ],
					"text" : "fl.or~",
					"varname" : "fl.or~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-23",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 103.0, 100.0, 22.0 ],
					"text" : "fl.plus~",
					"varname" : "fl.plus~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-25",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 133.0, 100.0, 22.0 ],
					"text" : "fl.pow~",
					"varname" : "fl.pow~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-27",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 163.0, 100.0, 22.0 ],
					"text" : "fl.times~",
					"varname" : "fl.times~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-28",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 3,
					"outlettype" : [ "", "", "" ],
					"patching_rect" : [ 30.0, 193.0, 100.0, 22.0 ],
					"text" : "fl.info~",
					"varname" : "fl.info~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-29",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 30.0, 223.0, 100.0, 22.0 ],
					"text" : "fl.read~",
					"varname" : "fl.read~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-30",
					"maxclass" : "newobj",
					"numinlets" : 4,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 30.0, 253.0, 102.0, 22.0 ],
					"text" : "fl.complexdivide~",
					"varname" : "fl.complexdivide~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-31",
					"maxclass" : "newobj",
					"numinlets" : 4,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 150.0, 283.0, 110.0, 22.0 ],
					"text" : "fl.complexmultiply~",
					"varname" : "fl.complexmultiply~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-32",
					"maxclass" : "newobj",
					"numinlets" : 4,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 150.0, 313.0, 100.0, 22.0 ],
					"text" : "fl.complexplus~",
					"varname" : "fl.complexplus~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-33",
					"maxclass" : "newobj",
					"numinlets" : 4,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 150.0, 343.0, 100.0, 22.0 ],
					"text" : "fl.complexpow~",
					"varname" : "fl.complexpow~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-34",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 150.0, 373.0, 100.0, 22.0 ],
					"text" : "fl.cartopol~",
					"varname" : "fl.cartopol~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-35",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 150.0, 403.0, 100.0, 22.0 ],
					"text" : "fl.complexconj~",
					"varname" : "fl.complexconj~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-36",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 150.0, 433.0, 100.0, 22.0 ],
					"text" : "fl.complexcos~",
					"varname" : "fl.complexcos~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-37",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 150.0, 463.0, 100.0, 22.0 ],
					"text" : "fl.complexcosh~",
					"varname" : "fl.complexcosh~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-38",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 150.0, 43.0, 100.0, 22.0 ],
					"text" : "fl.complexexp~",
					"varname" : "fl.complexexp~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-39",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 150.0, 73.0, 100.0, 22.0 ],
					"text" : "fl.complexlog~",
					"varname" : "fl.complexlog~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-40",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 150.0, 103.0, 100.0, 22.0 ],
					"text" : "fl.complexlog10~",
					"varname" : "fl.complexlog10~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-41",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 150.0, 133.0, 100.0, 22.0 ],
					"text" : "fl.complexsin~",
					"varname" : "fl.complexsin~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-42",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 150.0, 163.0, 100.0, 22.0 ],
					"text" : "fl.complexsinh~",
					"varname" : "fl.complexsinh~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-43",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 150.0, 193.0, 100.0, 22.0 ],
					"text" : "fl.complexsqrt~",
					"varname" : "fl.complexsqrt~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-44",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 150.0, 223.0, 100.0, 22.0 ],
					"text" : "fl.complextan~",
					"varname" : "fl.complextan~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-45",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 150.0, 253.0, 100.0, 22.0 ],
					"text" : "fl.complextanh~",
					"varname" : "fl.complextanh~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-46",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 270.0, 283.0, 100.0, 22.0 ],
					"text" : "fl.poltocar~",
					"varname" : "fl.poltocar~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-47",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 270.0, 313.0, 100.0, 22.0 ],
					"text" : "fl.complexexpr~",
					"varname" : "fl.complexexpr~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-48",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 270.0, 343.0, 100.0, 22.0 ],
					"text" : "fl.expr~",
					"varname" : "fl.expr~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-49",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 270.0, 373.0, 100.0, 22.0 ],
					"text" : "fl.0dfsvf~",
					"varname" : "fl.0dfsvf~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-50",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 270.0, 403.0, 100.0, 22.0 ],
					"text" : "fl.onepole~",
					"varname" : "fl.onepole~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-51",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 270.0, 433.0, 100.0, 22.0 ],
					"text" : "fl.onepolezero~",
					"varname" : "fl.onepolezero~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-52",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 270.0, 463.0, 100.0, 22.0 ],
					"text" : "fl.resonant~",
					"varname" : "fl.resonant~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-53",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 270.0, 43.0, 100.0, 22.0 ],
					"text" : "fl.sallenkey~",
					"varname" : "fl.sallenkey~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-54",
					"maxclass" : "newobj",
					"numinlets" : 3,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 270.0, 73.0, 100.0, 22.0 ],
					"text" : "fl.gaussian~",
					"varname" : "fl.gaussian~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-55",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 270.0, 103.0, 100.0, 22.0 ],
					"text" : "fl.ramp~",
					"varname" : "fl.ramp~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-56",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 270.0, 133.0, 100.0, 22.0 ],
					"text" : "fl.random~",
					"varname" : "fl.random~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-57",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 270.0, 163.0, 100.0, 22.0 ],
					"text" : "fl.uniform~",
					"varname" : "fl.uniform~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-58",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 270.0, 193.0, 100.0, 22.0 ],
					"text" : "fl.frommax~",
					"varname" : "fl.frommax~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-59",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 270.0, 223.0, 100.0, 22.0 ],
					"text" : "fl.tomax~",
					"varname" : "fl.tomax~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-60",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "signal" ],
					"patching_rect" : [ 270.0, 253.0, 100.0, 22.0 ],
					"text" : "fl.sink~",
					"varname" : "fl.sink~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-61",
					"maxclass" : "newobj",
					"numinlets" : 3,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 390.0, 283.0, 100.0, 22.0 ],
					"text" : "fl.source~",
					"varname" : "fl.source~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-62",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "signal" ],
					"patching_rect" : [ 390.0, 313.0, 100.0, 22.0 ],
					"text" : "fl.trace~",
					"varname" : "fl.trace~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-63",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 390.0, 343.0, 100.0, 22.0 ],
					"text" : "fl.constant~",
					"varname" : "fl.constant~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-64",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 390.0, 373.0, 100.0, 22.0 ],
					"text" : "fl.convert~",
					"varname" : "fl.convert~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-65",
					"maxclass" : "newobj",
					"numinlets" : 3,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 390.0, 403.0, 100.0, 22.0 ],
					"text" : "fl.lookup~",
					"varname" : "fl.lookup~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-66",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 390.0, 433.0, 100.0, 22.0 ],
					"text" : "fl.map~",
					"varname" : "fl.map~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-67",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 390.0, 463.0, 100.0, 22.0 ],
					"text" : "fl.samplerate~",
					"varname" : "fl.samplerate~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-68",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 390.0, 43.0, 100.0, 22.0 ],
					"text" : "fl.combinetags~",
					"varname" : "fl.combinetags~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-69",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 390.0, 73.0, 100.0, 22.0 ],
					"text" : "fl.filtertags~",
					"varname" : "fl.filtertags~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-70",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 390.0, 103.0, 100.0, 22.0 ],
					"text" : "fl.getparam~",
					"varname" : "fl.getparam~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-71",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 390.0, 133.0, 100.0, 22.0 ],
					"text" : "fl.tag~",
					"varname" : "fl.tag~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-72",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 390.0, 163.0, 100.0, 22.0 ],
					"text" : "fl.untag~",
					"varname" : "fl.untag~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-73",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 390.0, 193.0, 100.0, 22.0 ],
					"text" : "fl.route~",
					"varname" : "fl.route~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-74",
					"maxclass" : "newobj",
					"numinlets" : 3,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 390.0, 223.0, 100.0, 22.0 ],
					"text" : "fl.select~",
					"varname" : "fl.select~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-75",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 390.0, 253.0, 100.0, 22.0 ],
					"text" : "fl.audiotrigger~",
					"varname" : "fl.audiotrigger~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-76",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 510.0, 283.0, 100.0, 22.0 ],
					"text" : "fl.future~",
					"varname" : "fl.future~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-77",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 510.0, 313.0, 100.0, 22.0 ],
					"text" : "fl.interval~",
					"varname" : "fl.interval~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-78",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 510.0, 343.0, 100.0, 22.0 ],
					"text" : "fl.once~",
					"varname" : "fl.once~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-79",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 510.0, 373.0, 100.0, 22.0 ],
					"text" : "fl.perblock~",
					"varname" : "fl.perblock~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-80",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 510.0, 403.0, 120.0, 22.0 ],
					"text" : "fl.coordinatesystem~",
					"varname" : "fl.coordinatesystem~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-81",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 510.0, 433.0, 100.0, 22.0 ],
					"text" : "fl.spatial~",
					"varname" : "fl.spatial~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-82",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 510.0, 463.0, 100.0, 22.0 ],
					"text" : "fl.convolve~",
					"varname" : "fl.convolve~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-83",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 510.0, 43.0, 100.0, 22.0 ],
					"text" : "fl.correlate~",
					"varname" : "fl.correlate~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-84",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 510.0, 73.0, 100.0, 22.0 ],
					"text" : "fl.fft~",
					"varname" : "fl.fft~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-85",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 510.0, 103.0, 100.0, 22.0 ],
					"text" : "fl.ifft~",
					"varname" : "fl.ifft~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-86",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 510.0, 133.0, 100.0, 22.0 ],
					"text" : "fl.multitaper~",
					"varname" : "fl.multitaper~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-87",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 510.0, 163.0, 100.0, 22.0 ],
					"text" : "fl.window~",
					"varname" : "fl.window~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-88",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 510.0, 193.0, 100.0, 22.0 ],
					"text" : "fl.recall~",
					"varname" : "fl.recall~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-90",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 510.0, 223.0, 100.0, 22.0 ],
					"text" : "fl.register~",
					"varname" : "fl.register~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-91",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 510.0, 253.0, 100.0, 22.0 ],
					"text" : "fl.store~",
					"varname" : "fl.store~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-93",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 630.0, 283.0, 100.0, 22.0 ],
					"text" : "fl.pack~",
					"varname" : "fl.pack~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-96",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 630.0, 313.0, 100.0, 22.0 ],
					"text" : "fl.streamid~",
					"varname" : "fl.streamid~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-97",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 630.0, 343.0, 100.0, 22.0 ],
					"text" : "fl.unpack~",
					"varname" : "fl.unpack~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-98",
					"maxclass" : "newobj",
					"numinlets" : 3,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 630.0, 373.0, 100.0, 22.0 ],
					"text" : "fl.clip~",
					"varname" : "fl.clip~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-99",
					"maxclass" : "newobj",
					"numinlets" : 3,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 630.0, 403.0, 100.0, 22.0 ],
					"text" : "fl.fold~",
					"varname" : "fl.fold~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-100",
					"maxclass" : "newobj",
					"numinlets" : 3,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 630.0, 433.0, 100.0, 22.0 ],
					"text" : "fl.wrap~",
					"varname" : "fl.wrap~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-101",
					"maxclass" : "newobj",
					"numinlets" : 3,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 630.0, 463.0, 100.0, 22.0 ],
					"text" : "fl.clip~",
					"varname" : "fl.clip~[1]"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-102",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 630.0, 43.0, 100.0, 22.0 ],
					"text" : "fl.ewma~",
					"varname" : "fl.ewma~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-104",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 630.0, 73.0, 100.0, 22.0 ],
					"text" : "fl.ewmsd~",
					"varname" : "fl.ewmsd~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-105",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 630.0, 103.0, 100.0, 22.0 ],
					"text" : "fl.framedelta~",
					"varname" : "fl.framedelta~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-106",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 630.0, 133.0, 100.0, 22.0 ],
					"text" : "fl.lag~",
					"varname" : "fl.lag~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-107",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 630.0, 163.0, 100.0, 22.0 ],
					"text" : "fl.timemean~",
					"varname" : "fl.timemean~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-110",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 630.0, 193.0, 100.0, 22.0 ],
					"text" : "fl.timemedian~",
					"varname" : "fl.timemedian~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-111",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 630.0, 223.0, 100.0, 22.0 ],
					"text" : "fl.timestddev~",
					"varname" : "fl.timestddev~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-112",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 630.0, 253.0, 100.0, 22.0 ],
					"text" : "fl.now~",
					"varname" : "fl.now~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-113",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 750.0, 283.0, 100.0, 22.0 ],
					"text" : "fl.ticks~",
					"varname" : "fl.ticks~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-114",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 750.0, 313.0, 100.0, 22.0 ],
					"text" : "fl.timedelta~",
					"varname" : "fl.timedelta~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-115",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 750.0, 343.0, 100.0, 22.0 ],
					"text" : "fl.timer~",
					"varname" : "fl.timer~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-116",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 750.0, 373.0, 100.0, 22.0 ],
					"text" : "fl.abs~",
					"varname" : "fl.abs~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-117",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 750.0, 403.0, 100.0, 22.0 ],
					"text" : "fl.acos~",
					"varname" : "fl.acos~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-118",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 750.0, 433.0, 100.0, 22.0 ],
					"text" : "fl.acosh~",
					"varname" : "fl.acosh~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-119",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 750.0, 463.0, 100.0, 22.0 ],
					"text" : "fl.asin~",
					"varname" : "fl.asin~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-120",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 750.0, 43.0, 100.0, 22.0 ],
					"text" : "fl.asinh~",
					"varname" : "fl.asinh~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-121",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 750.0, 73.0, 100.0, 22.0 ],
					"text" : "fl.atan~",
					"varname" : "fl.atan~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-122",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 750.0, 103.0, 100.0, 22.0 ],
					"text" : "fl.atanh~",
					"varname" : "fl.atanh~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-123",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 750.0, 133.0, 100.0, 22.0 ],
					"text" : "fl.cbrt~",
					"varname" : "fl.cbrt~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-124",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 750.0, 163.0, 100.0, 22.0 ],
					"text" : "fl.ceil~",
					"varname" : "fl.ceil~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-125",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 750.0, 193.0, 100.0, 22.0 ],
					"text" : "fl.cos~",
					"varname" : "fl.cos~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-126",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 750.0, 223.0, 100.0, 22.0 ],
					"text" : "fl.cosh~",
					"varname" : "fl.cosh~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-127",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 750.0, 253.0, 100.0, 22.0 ],
					"text" : "fl.erf~",
					"varname" : "fl.erf~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-128",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 870.0, 283.0, 100.0, 22.0 ],
					"text" : "fl.erfc~",
					"varname" : "fl.erfc~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-129",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 870.0, 313.0, 100.0, 22.0 ],
					"text" : "fl.exp~",
					"varname" : "fl.exp~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-130",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 870.0, 343.0, 100.0, 22.0 ],
					"text" : "fl.exp2~",
					"varname" : "fl.exp2~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-131",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 870.0, 373.0, 100.0, 22.0 ],
					"text" : "fl.floor~",
					"varname" : "fl.floor~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-132",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 870.0, 403.0, 100.0, 22.0 ],
					"text" : "fl.log~",
					"varname" : "fl.log~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-133",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 870.0, 433.0, 100.0, 22.0 ],
					"text" : "fl.log10~",
					"varname" : "fl.log10~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-134",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 870.0, 463.0, 100.0, 22.0 ],
					"text" : "fl.not~",
					"varname" : "fl.not~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-135",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 870.0, 43.0, 100.0, 22.0 ],
					"text" : "fl.round~",
					"varname" : "fl.round~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-136",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 870.0, 73.0, 100.0, 22.0 ],
					"text" : "fl.sin~",
					"varname" : "fl.sin~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-137",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 870.0, 103.0, 100.0, 22.0 ],
					"text" : "fl.sinh~",
					"varname" : "fl.sinh~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-138",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 870.0, 133.0, 100.0, 22.0 ],
					"text" : "fl.sqrt~",
					"varname" : "fl.sqrt~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-139",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 870.0, 163.0, 100.0, 22.0 ],
					"text" : "fl.tan~",
					"varname" : "fl.tan~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-140",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 870.0, 193.0, 100.0, 22.0 ],
					"text" : "fl.tanh~",
					"varname" : "fl.tanh~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-141",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 870.0, 223.0, 100.0, 22.0 ],
					"text" : "fl.trunc~",
					"varname" : "fl.trunc~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-142",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 870.0, 253.0, 100.0, 22.0 ],
					"text" : "fl.accumpoint~",
					"varname" : "fl.accumpoint~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-143",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 990.0, 283.0, 100.0, 22.0 ],
					"text" : "fl.argmax~",
					"varname" : "fl.argmax~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-144",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 990.0, 313.0, 100.0, 22.0 ],
					"text" : "fl.argmin~",
					"varname" : "fl.argmin~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-145",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 990.0, 343.0, 100.0, 22.0 ],
					"text" : "fl.centroid~",
					"varname" : "fl.centroid~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-146",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 990.0, 373.0, 100.0, 22.0 ],
					"text" : "fl.chop~",
					"varname" : "fl.chop~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-147",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 990.0, 403.0, 100.0, 22.0 ],
					"text" : "fl.crest~",
					"varname" : "fl.crest~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-148",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 990.0, 433.0, 100.0, 22.0 ],
					"text" : "fl.flatness~",
					"varname" : "fl.flatness~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-149",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 990.0, 463.0, 108.0, 22.0 ],
					"text" : "fl.geometricmean~",
					"varname" : "fl.geometricmean~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-150",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 990.0, 43.0, 100.0, 22.0 ],
					"text" : "fl.join~",
					"varname" : "fl.join~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-151",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 990.0, 73.0, 100.0, 22.0 ],
					"text" : "fl.kurtosis~",
					"varname" : "fl.kurtosis~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-152",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 990.0, 103.0, 100.0, 22.0 ],
					"text" : "fl.length~",
					"varname" : "fl.length~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-153",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 990.0, 133.0, 100.0, 22.0 ],
					"text" : "fl.mean~",
					"varname" : "fl.mean~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-154",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 990.0, 163.0, 100.0, 22.0 ],
					"text" : "fl.medianfilter~",
					"varname" : "fl.medianfilter~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-155",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 990.0, 193.0, 100.0, 22.0 ],
					"text" : "fl.nanfilter~",
					"varname" : "fl.nanfilter~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-156",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 990.0, 223.0, 100.0, 22.0 ],
					"text" : "fl.nonzero~",
					"varname" : "fl.nonzero~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-157",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 990.0, 253.0, 100.0, 22.0 ],
					"text" : "fl.pad~",
					"varname" : "fl.pad~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-158",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 3,
					"outlettype" : [ "", "", "" ],
					"patching_rect" : [ 1110.0, 283.0, 100.0, 22.0 ],
					"text" : "fl.peaks~",
					"varname" : "fl.peaks~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-159",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 1110.0, 313.0, 100.0, 22.0 ],
					"text" : "fl.percentile~",
					"varname" : "fl.percentile~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-160",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 1110.0, 343.0, 100.0, 22.0 ],
					"text" : "fl.product~",
					"varname" : "fl.product~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-161",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 1110.0, 373.0, 100.0, 22.0 ],
					"text" : "fl.reverse~",
					"varname" : "fl.reverse~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-162",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 1110.0, 403.0, 100.0, 22.0 ],
					"text" : "fl.rms~",
					"varname" : "fl.rms~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-163",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 1110.0, 433.0, 100.0, 22.0 ],
					"text" : "fl.shift~",
					"varname" : "fl.shift~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-164",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 1110.0, 463.0, 100.0, 22.0 ],
					"text" : "fl.skewness~",
					"varname" : "fl.skewness~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-165",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 1110.0, 43.0, 100.0, 22.0 ],
					"text" : "fl.sort~",
					"varname" : "fl.sort~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-166",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 1110.0, 73.0, 100.0, 22.0 ],
					"text" : "fl.split~",
					"varname" : "fl.split~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-167",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 1110.0, 103.0, 100.0, 22.0 ],
					"text" : "fl.spread~",
					"varname" : "fl.spread~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-168",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 1110.0, 133.0, 120.0, 22.0 ],
					"text" : "fl.standarddeviation~",
					"varname" : "fl.standarddeviation~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-169",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 1110.0, 163.0, 100.0, 22.0 ],
					"text" : "fl.subframe~",
					"varname" : "fl.subframe~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-170",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 1110.0, 193.0, 100.0, 22.0 ],
					"text" : "fl.sum~",
					"varname" : "fl.sum~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-171",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 1110.0, 223.0, 100.0, 22.0 ],
					"text" : "fl.vmax~",
					"varname" : "fl.vmax~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-172",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 1110.0, 253.0, 100.0, 22.0 ],
					"text" : "fl.vmin~",
					"varname" : "fl.vmin~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-173",
					"maxclass" : "newobj",
					"numinlets" : 3,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 1230.0, 283.0, 100.0, 22.0 ],
					"text" : "fl.dispatch~",
					"varname" : "fl.dispatch~"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-109", 0 ],
					"source" : [ "obj-103", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-103", 0 ],
					"source" : [ "obj-108", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-21", 0 ],
					"source" : [ "obj-109", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-22", 0 ],
					"source" : [ "obj-20", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-26", 0 ],
					"source" : [ "obj-21", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-95", 0 ],
					"source" : [ "obj-21", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 39.5, 31.0, 39.5, 31.0 ],
					"order" : 156,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-10", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 247.0, 39.5, 247.0 ],
					"order" : 142,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-100", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 627.0, 496.0, 627.0, 430.0, 639.5, 430.0 ],
					"order" : 61,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-102", 0 ],
					"midpoints" : [ 39.5, 37.0, 639.5, 37.0 ],
					"order" : 74,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-104", 0 ],
					"midpoints" : [ 39.5, 40.0, 627.0, 40.0, 627.0, 67.0, 639.5, 67.0 ],
					"order" : 73,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-105", 0 ],
					"midpoints" : [ 39.5, 40.0, 627.0, 40.0, 627.0, 97.0, 639.5, 97.0 ],
					"order" : 72,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-106", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 127.0, 639.5, 127.0 ],
					"order" : 71,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-107", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 157.0, 639.5, 157.0 ],
					"order" : 70,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-11", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 277.0, 39.5, 277.0 ],
					"order" : 141,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-110", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 187.0, 639.5, 187.0 ],
					"order" : 69,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-111", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 217.0, 639.5, 217.0 ],
					"order" : 68,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-112", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 247.0, 639.5, 247.0 ],
					"order" : 67,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-113", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 747.0, 496.0, 747.0, 280.0, 759.5, 280.0 ],
					"order" : 52,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-114", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 747.0, 496.0, 747.0, 310.0, 759.5, 310.0 ],
					"order" : 51,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-115", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 747.0, 496.0, 747.0, 340.0, 759.5, 340.0 ],
					"order" : 50,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-116", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 747.0, 496.0, 747.0, 370.0, 759.5, 370.0 ],
					"order" : 49,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-117", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 747.0, 496.0, 747.0, 400.0, 759.5, 400.0 ],
					"order" : 48,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-118", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 747.0, 496.0, 747.0, 430.0, 759.5, 430.0 ],
					"order" : 47,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-119", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 747.0, 496.0, 747.0, 460.0, 759.5, 460.0 ],
					"order" : 46,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-12", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 307.0, 39.5, 307.0 ],
					"order" : 140,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-120", 0 ],
					"midpoints" : [ 39.5, 37.0, 759.5, 37.0 ],
					"order" : 60,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-121", 0 ],
					"midpoints" : [ 39.5, 40.0, 747.0, 40.0, 747.0, 67.0, 759.5, 67.0 ],
					"order" : 59,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-122", 0 ],
					"midpoints" : [ 39.5, 40.0, 747.0, 40.0, 747.0, 97.0, 759.5, 97.0 ],
					"order" : 58,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-123", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 127.0, 759.5, 127.0 ],
					"order" : 57,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-124", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 157.0, 759.5, 157.0 ],
					"order" : 56,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-125", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 187.0, 759.5, 187.0 ],
					"order" : 55,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-126", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 217.0, 759.5, 217.0 ],
					"order" : 54,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-127", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 247.0, 759.5, 247.0 ],
					"order" : 53,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-128", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 867.0, 496.0, 867.0, 280.0, 879.5, 280.0 ],
					"order" : 37,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-129", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 867.0, 496.0, 867.0, 310.0, 879.5, 310.0 ],
					"order" : 36,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 337.0, 39.5, 337.0 ],
					"order" : 139,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-130", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 867.0, 496.0, 867.0, 340.0, 879.5, 340.0 ],
					"order" : 35,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-131", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 867.0, 496.0, 867.0, 370.0, 879.5, 370.0 ],
					"order" : 34,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-132", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 867.0, 496.0, 867.0, 400.0, 879.5, 400.0 ],
					"order" : 33,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-133", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 867.0, 496.0, 867.0, 430.0, 879.5, 430.0 ],
					"order" : 32,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-134", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 867.0, 496.0, 867.0, 460.0, 879.5, 460.0 ],
					"order" : 31,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-135", 0 ],
					"midpoints" : [ 39.5, 37.0, 879.5, 37.0 ],
					"order" : 45,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-136", 0 ],
					"midpoints" : [ 39.5, 40.0, 867.0, 40.0, 867.0, 67.0, 879.5, 67.0 ],
					"order" : 44,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-137", 0 ],
					"midpoints" : [ 39.5, 40.0, 867.0, 40.0, 867.0, 97.0, 879.5, 97.0 ],
					"order" : 43,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-138", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 127.0, 879.5, 127.0 ],
					"order" : 42,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-139", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 157.0, 879.5, 157.0 ],
					"order" : 41,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-14", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 367.0, 39.5, 367.0 ],
					"order" : 138,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-140", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 187.0, 879.5, 187.0 ],
					"order" : 40,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-141", 0 ],
					"midpoints" : [ 39.5, 126.166675567626953, 879.5, 126.166675567626953 ],
					"order" : 39,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-142", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 867.0, 496.0, 867.0, 250.0, 879.5, 250.0 ],
					"order" : 38,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-143", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 987.0, 496.0, 987.0, 280.0, 999.5, 280.0 ],
					"order" : 22,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-144", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 987.0, 496.0, 987.0, 310.0, 999.5, 310.0 ],
					"order" : 21,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-145", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 987.0, 496.0, 987.0, 340.0, 999.5, 340.0 ],
					"order" : 20,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-146", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 987.0, 496.0, 987.0, 370.0, 999.5, 370.0 ],
					"order" : 19,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-147", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 987.0, 496.0, 987.0, 400.0, 999.5, 400.0 ],
					"order" : 18,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-148", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 987.0, 496.0, 987.0, 430.0, 999.5, 430.0 ],
					"order" : 17,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-149", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 987.0, 496.0, 987.0, 460.0, 999.5, 460.0 ],
					"order" : 16,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-15", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 397.0, 39.5, 397.0 ],
					"order" : 137,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-150", 0 ],
					"order" : 30,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-151", 0 ],
					"midpoints" : [ 39.5, 40.0, 987.0, 40.0, 987.0, 67.0, 999.5, 67.0 ],
					"order" : 29,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-152", 0 ],
					"midpoints" : [ 39.5, 40.0, 987.0, 40.0, 987.0, 97.0, 999.5, 97.0 ],
					"order" : 28,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-153", 0 ],
					"midpoints" : [ 39.5, 81.166675567626953, 999.5, 81.166675567626953 ],
					"order" : 27,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-154", 0 ],
					"midpoints" : [ 39.5, 96.166675567626953, 999.5, 96.166675567626953 ],
					"order" : 26,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-155", 0 ],
					"midpoints" : [ 39.5, 111.166675567626953, 999.5, 111.166675567626953 ],
					"order" : 25,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-156", 0 ],
					"midpoints" : [ 39.5, 126.166675567626953, 999.5, 126.166675567626953 ],
					"order" : 24,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-157", 0 ],
					"midpoints" : [ 39.5, 141.166675567626953, 999.5, 141.166675567626953 ],
					"order" : 23,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-158", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 1107.0, 496.0, 1107.0, 280.0, 1119.5, 280.0 ],
					"order" : 7,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-159", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 1107.0, 496.0, 1107.0, 310.0, 1119.5, 310.0 ],
					"order" : 6,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-16", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 427.0, 39.5, 427.0 ],
					"order" : 136,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-160", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 1107.0, 496.0, 1107.0, 340.0, 1119.5, 340.0 ],
					"order" : 5,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-161", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 1107.0, 496.0, 1107.0, 370.0, 1119.5, 370.0 ],
					"order" : 4,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-162", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 1107.0, 496.0, 1107.0, 400.0, 1119.5, 400.0 ],
					"order" : 3,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-163", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 1107.0, 496.0, 1107.0, 430.0, 1119.5, 430.0 ],
					"order" : 2,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-164", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 1107.0, 496.0, 1107.0, 460.0, 1119.5, 460.0 ],
					"order" : 1,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-165", 0 ],
					"order" : 15,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-166", 0 ],
					"midpoints" : [ 39.5, 51.166675567626953, 1119.5, 51.166675567626953 ],
					"order" : 14,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-167", 0 ],
					"midpoints" : [ 39.5, 66.166675567626953, 1119.5, 66.166675567626953 ],
					"order" : 13,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-168", 0 ],
					"midpoints" : [ 39.5, 81.166675567626953, 1119.5, 81.166675567626953 ],
					"order" : 12,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-169", 0 ],
					"midpoints" : [ 39.5, 96.166675567626953, 1119.5, 96.166675567626953 ],
					"order" : 11,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 457.0, 39.5, 457.0 ],
					"order" : 135,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-170", 0 ],
					"midpoints" : [ 39.5, 111.166675567626953, 1119.5, 111.166675567626953 ],
					"order" : 10,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-171", 0 ],
					"midpoints" : [ 39.5, 126.166675567626953, 1119.5, 126.166675567626953 ],
					"order" : 9,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-172", 0 ],
					"midpoints" : [ 39.5, 141.166675567626953, 1119.5, 141.166675567626953 ],
					"order" : 8,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-173", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 1227.0, 496.0, 1227.0, 280.0, 1239.5, 280.0 ],
					"order" : 0,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-18", 0 ],
					"midpoints" : [ 39.5, 31.0, 39.5, 31.0 ],
					"order" : 157,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-19", 0 ],
					"midpoints" : [ 39.5, 40.0, 27.0, 40.0, 27.0, 67.0, 39.5, 67.0 ],
					"order" : 155,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 0 ],
					"midpoints" : [ 39.5, 40.0, 27.0, 40.0, 27.0, 67.0, 39.5, 67.0 ],
					"order" : 154,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-23", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 97.0, 39.5, 97.0 ],
					"order" : 153,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-25", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 127.0, 39.5, 127.0 ],
					"order" : 151,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-27", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 157.0, 39.5, 157.0 ],
					"order" : 149,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-28", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 187.0, 39.5, 187.0 ],
					"order" : 147,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-29", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 217.0, 39.5, 217.0 ],
					"order" : 145,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-3", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 97.0, 39.5, 97.0 ],
					"order" : 152,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-30", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 247.0, 39.5, 247.0 ],
					"order" : 143,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-31", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 277.0, 159.5, 277.0 ],
					"order" : 126,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-32", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 307.0, 159.5, 307.0 ],
					"order" : 125,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-33", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 337.0, 159.5, 337.0 ],
					"order" : 124,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-34", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 367.0, 159.5, 367.0 ],
					"order" : 123,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-35", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 397.0, 159.5, 397.0 ],
					"order" : 122,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-36", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 427.0, 159.5, 427.0 ],
					"order" : 121,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-37", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 147.0, 496.0, 147.0, 460.0, 159.5, 460.0 ],
					"order" : 120,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-38", 0 ],
					"midpoints" : [ 39.5, 37.0, 159.5, 37.0 ],
					"order" : 134,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-39", 0 ],
					"midpoints" : [ 39.5, 40.0, 147.0, 40.0, 147.0, 67.0, 159.5, 67.0 ],
					"order" : 133,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 127.0, 39.5, 127.0 ],
					"order" : 150,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-40", 0 ],
					"midpoints" : [ 39.5, 40.0, 147.0, 40.0, 147.0, 97.0, 159.5, 97.0 ],
					"order" : 132,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-41", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 127.0, 159.5, 127.0 ],
					"order" : 131,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-42", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 157.0, 159.5, 157.0 ],
					"order" : 130,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-43", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 187.0, 159.5, 187.0 ],
					"order" : 129,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-44", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 217.0, 159.5, 217.0 ],
					"order" : 128,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-45", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 247.0, 159.5, 247.0 ],
					"order" : 127,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-46", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 277.0, 279.5, 277.0 ],
					"order" : 111,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-47", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 307.0, 279.5, 307.0 ],
					"order" : 110,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-48", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 337.0, 279.5, 337.0 ],
					"order" : 109,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-49", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 367.0, 279.5, 367.0 ],
					"order" : 108,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-5", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 157.0, 39.5, 157.0 ],
					"order" : 148,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-50", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 397.0, 279.5, 397.0 ],
					"order" : 107,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-51", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 267.0, 496.0, 267.0, 430.0, 279.5, 430.0 ],
					"order" : 106,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-52", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 267.0, 496.0, 267.0, 460.0, 279.5, 460.0 ],
					"order" : 105,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-53", 0 ],
					"midpoints" : [ 39.5, 37.0, 279.5, 37.0 ],
					"order" : 119,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-54", 0 ],
					"midpoints" : [ 39.5, 40.0, 267.0, 40.0, 267.0, 67.0, 279.5, 67.0 ],
					"order" : 118,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-55", 0 ],
					"midpoints" : [ 39.5, 40.0, 267.0, 40.0, 267.0, 97.0, 279.5, 97.0 ],
					"order" : 117,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-56", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 127.0, 279.5, 127.0 ],
					"order" : 116,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-57", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 157.0, 279.5, 157.0 ],
					"order" : 115,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-58", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 187.0, 279.5, 187.0 ],
					"order" : 114,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-59", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 217.0, 279.5, 217.0 ],
					"order" : 113,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-6", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 187.0, 39.5, 187.0 ],
					"order" : 146,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-60", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 247.0, 279.5, 247.0 ],
					"order" : 112,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-61", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 277.0, 399.5, 277.0 ],
					"order" : 96,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-62", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 307.0, 399.5, 307.0 ],
					"order" : 95,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-63", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 337.0, 399.5, 337.0 ],
					"order" : 94,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-64", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 367.0, 399.5, 367.0 ],
					"order" : 93,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-65", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 387.0, 496.0, 387.0, 400.0, 399.5, 400.0 ],
					"order" : 92,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-66", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 387.0, 496.0, 387.0, 430.0, 399.5, 430.0 ],
					"order" : 91,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-67", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 387.0, 496.0, 387.0, 460.0, 399.5, 460.0 ],
					"order" : 90,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-68", 0 ],
					"midpoints" : [ 39.5, 37.0, 399.5, 37.0 ],
					"order" : 104,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-69", 0 ],
					"midpoints" : [ 39.5, 40.0, 387.0, 40.0, 387.0, 67.0, 399.5, 67.0 ],
					"order" : 103,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-7", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 217.0, 39.5, 217.0 ],
					"order" : 144,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-70", 0 ],
					"midpoints" : [ 39.5, 40.0, 387.0, 40.0, 387.0, 97.0, 399.5, 97.0 ],
					"order" : 102,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-71", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 127.0, 399.5, 127.0 ],
					"order" : 101,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-72", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 157.0, 399.5, 157.0 ],
					"order" : 100,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-73", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 187.0, 399.5, 187.0 ],
					"order" : 99,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-74", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 217.0, 399.5, 217.0 ],
					"order" : 98,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-75", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 247.0, 399.5, 247.0 ],
					"order" : 97,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-76", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 277.0, 519.5, 277.0 ],
					"order" : 81,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-77", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 307.0, 519.5, 307.0 ],
					"order" : 80,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-78", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 507.0, 496.0, 507.0, 340.0, 519.5, 340.0 ],
					"order" : 79,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-79", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 507.0, 496.0, 507.0, 370.0, 519.5, 370.0 ],
					"order" : 78,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-80", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 507.0, 496.0, 507.0, 400.0, 519.5, 400.0 ],
					"order" : 77,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-81", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 507.0, 496.0, 507.0, 430.0, 519.5, 430.0 ],
					"order" : 76,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-82", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 507.0, 496.0, 507.0, 460.0, 519.5, 460.0 ],
					"order" : 75,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-83", 0 ],
					"midpoints" : [ 39.5, 37.0, 519.5, 37.0 ],
					"order" : 89,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-84", 0 ],
					"midpoints" : [ 39.5, 40.0, 507.0, 40.0, 507.0, 67.0, 519.5, 67.0 ],
					"order" : 88,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-85", 0 ],
					"midpoints" : [ 39.5, 40.0, 507.0, 40.0, 507.0, 97.0, 519.5, 97.0 ],
					"order" : 87,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-86", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 127.0, 519.5, 127.0 ],
					"order" : 86,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-87", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 157.0, 519.5, 157.0 ],
					"order" : 85,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-88", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 187.0, 519.5, 187.0 ],
					"order" : 84,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-90", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 217.0, 519.5, 217.0 ],
					"order" : 83,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-91", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 247.0, 519.5, 247.0 ],
					"order" : 82,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-93", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 277.0, 639.5, 277.0 ],
					"order" : 66,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-96", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 307.0, 639.5, 307.0 ],
					"order" : 65,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-97", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 507.0, 496.0, 507.0, 340.0, 639.5, 340.0 ],
					"order" : 64,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-98", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 507.0, 496.0, 507.0, 370.0, 639.5, 370.0 ],
					"order" : 63,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-99", 0 ],
					"midpoints" : [ 39.5, 31.0, 15.0, 31.0, 15.0, 496.0, 507.0, 496.0, 507.0, 400.0, 639.5, 400.0 ],
					"order" : 62,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-20", 0 ],
					"source" : [ "obj-26", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-103", 0 ],
					"source" : [ "obj-585", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-92", 1 ],
					"source" : [ "obj-89", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-8", 0 ],
					"source" : [ "obj-9", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-94", 0 ],
					"source" : [ "obj-92", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-9", 0 ],
					"source" : [ "obj-94", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-92", 0 ],
					"source" : [ "obj-95", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-94", 2 ],
					"source" : [ "obj-95", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-94", 1 ],
					"source" : [ "obj-95", 1 ]
				}

			}
 ],
		"dependency_cache" : [ 			{
				"name" : "creator.js",
				"bootpath" : "~/Sync/00_MAX/_Max_Libs/_Abstractions/js",
				"patcherrelativepath" : "../../../Sync/00_MAX/_Max_Libs/_Abstractions/js",
				"type" : "TEXT",
				"implicit" : 1
			}
, 			{
				"name" : "fl.dispatch~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.vmin~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.vmax~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.sum~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.subframe~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.standarddeviation~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.spread~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.split~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.sort~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.skewness~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.shift~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.rms~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.reverse~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.product~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.percentile~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.peaks~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.pad~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.nonzero~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.nanfilter~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.medianfilter~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.mean~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.length~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.kurtosis~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.join~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.geometricmean~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.flatness~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.crest~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.chop~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.centroid~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.argmin~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.argmax~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.accumpoint~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.trunc~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.tanh~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.tan~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.sqrt~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.sinh~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.sin~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.round~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.not~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.log10~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.log~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.floor~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.exp2~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.exp~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.erfc~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.erf~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.cosh~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.cos~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.ceil~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.cbrt~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.atanh~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.atan~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.asinh~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.asin~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.acosh~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.acos~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.abs~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.timer~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.timedelta~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.ticks~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.now~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.timestddev~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.timemedian~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.timemean~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.lag~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.framedelta~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.ewmsd~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.ewma~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.clip~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.wrap~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.fold~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.unpack~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.streamid~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.pack~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.store~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.register~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.recall~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.window~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.multitaper~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.ifft~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.fft~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.correlate~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.convolve~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.spatial~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.coordinatesystem~.mxo",
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
				"name" : "fl.once~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.once~.mxo",
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
				"name" : "fl.future~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.future~.mxo",
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
				"name" : "fl.select~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.route~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.untag~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.tag~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.getparam~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.filtertags~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.combinetags~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.samplerate~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.map~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.lookup~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.convert~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.constant~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.trace~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.trace~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.source~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.source~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.sink~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.sink~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.tomax~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.frommax~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.uniform~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.random~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.ramp~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.gaussian~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.sallenkey~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.resonant~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.onepolezero~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.onepole~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.0dfsvf~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.expr~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.complexexpr~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.poltocar~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.complextanh~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.complextan~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.complexsqrt~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.complexsinh~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.complexsin~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.complexlog10~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.complexlog~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.complexexp~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.complexcosh~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.complexcos~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.complexconj~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.cartopol~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.complexpow~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.complexplus~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.complexmultiply~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.complexdivide~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.read~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.info~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.times~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.pow~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.plus~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.or~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.notequals~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.modulo~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.minus~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.min~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.max~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.lessthaneq~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.lessthan~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.hypot~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.greaterthaneq~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.greaterthan~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.equals~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.divide~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.diff~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.copysign~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.atan2~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "fl.and~.mxo",
				"type" : "iLaX"
			}
 ],
		"autosave" : 0
	}

}
