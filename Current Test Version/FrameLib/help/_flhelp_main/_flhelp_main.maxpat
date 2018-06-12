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
		"rect" : [ 0.0, 44.0, 759.0, 856.0 ],
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
		"boxes" : [ 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"hidden" : 1,
					"id" : "obj-3",
					"maxclass" : "newobj",
					"numinlets" : 1,
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
						"rect" : [ 50.0, 94.0, 264.0, 148.0 ],
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
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-2",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 99.0, 63.5, 83.0, 22.0 ],
									"style" : "",
									"text" : "prepend help",
									"varname" : "phelp"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-3",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 99.0, 90.5, 56.0, 22.0 ],
									"style" : "",
									"text" : "pcontrol"
								}

							}
, 							{
								"box" : 								{
									"comment" : "From umenu label out",
									"id" : "obj-4",
									"index" : 1,
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 99.0, 36.0, 25.0, 25.0 ],
									"style" : ""
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-3", 0 ],
									"source" : [ "obj-2", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-2", 0 ],
									"source" : [ "obj-4", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 67.5, 105.0, 73.0, 22.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"style" : "",
						"tags" : ""
					}
,
					"style" : "",
					"text" : "p load_help"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-7",
					"items" : [ "(Frame", "Lib", "Objects)", ",", "<separator>", ",", "(Schedulers)", ",", "<separator>", ",", "fl.audiotrigger~", ",", "fl.perblock~", ",", "fl.future~", ",", "fl.interval~", ",", "fl.once~", ",", "<separator>", ",", "(Unary)", ",", "<separator>", ",", "fl.abs~", ",", "fl.acosh~", ",", "fl.acos~", ",", "fl.asinh~", ",", "fl.asin~", ",", "fl.atanh~", ",", "fl.atan~", ",", "fl.cbrt~", ",", "fl.ceil~", ",", "fl.cosh~", ",", "fl.cos~", ",", "fl.erfc~", ",", "fl.erf~", ",", "fl.exp2~", ",", "fl.exp~", ",", "fl.floor~", ",", "fl.log10~", ",", "fl.log2~", ",", "fl.log~", ",", "fl.not~", ",", "fl.round~", ",", "fl.sinh~", ",", "fl.sin~", ",", "fl.sqrt~", ",", "fl.tanh~", ",", "fl.tan~", ",", "fl.trunc~", ",", "<separator>", ",", "(Binary)", ",", "<separator>", ",", "fl.and~", ",", "fl.atan2~", ",", "fl.copysign~", ",", "fl.diff~", ",", "fl.divide~", ",", "fl.equals~", ",", "fl.greaterthaneq~", ",", "fl.greaterthan~", ",", "fl.hypot~", ",", "fl.lessthaneq~", ",", "fl.lessthan~", ",", "fl.max~", ",", "fl.minus~", ",", "fl.min~", ",", "fl.modulo~", ",", "fl.notequals~", ",", "fl.or~", ",", "fl.plus~", ",", "fl.pow~", ",", "fl.times~", ",", "<separator>", ",", "(Vector)", ",", "<separator>", ",", "fl.centroid~", ",", "fl.chop~", ",", "fl.crest~", ",", "fl.flatness~", ",", "fl.geometricmean~", ",", "fl.join~", ",", "fl.kurtosis~", ",", "fl.length~", ",", "fl.mean~", ",", "fl.medianfilter~", ",", "fl.nonzero~", ",", "fl.pad~", ",", "fl.peaks~", ",", "fl.percentile~", ",", "fl.product~", ",", "fl.rms~", ",", "fl.shift~", ",", "fl.skewness~", ",", "fl.sort~", ",", "fl.split~", ",", "fl.spread~", ",", "fl.standarddeviation~", ",", "fl.subframe~", ",", "fl.sum~", ",", "fl.vmax~", ",", "fl.vmin~", ",", "<separator>", ",", "(Generators)", ",", "<separator>", ",", "fl.ramp~", ",", "fl.random~", ",", "fl.uniform~", ",", "<separator>", ",", "(IO)", ",", "<separator>", ",", "fl.sink~", ",", "fl.source~", ",", "fl.trace~", ",", "<separator>", ",", "(Mapping)", ",", "<separator>", ",", "fl.constant~", ",", "fl.convert~", ",", "fl.lookup~", ",", "fl.map~", ",", "fl.samplerate~", ",", "<separator>", ",", "(Max", "Specific)", ",", "<separator>", ",", "fl.frommax~", ",", "fl.tomax~", ",", "fl.read~", ",", "<separator>", ",", "(Parameters)", ",", "<separator>", ",", "fl.combinetags~", ",", "fl.filtertags~", ",", "fl.getparam~", ",", "fl.tag~", ",", "fl.untag~", "<separator>", ",", "(Routing)", ",", "<separator>", ",", "fl.dispatch~", ",", "fl.route~", ",", "fl.select~", ",", "<separator>", ",", "(Streaming)", ",", "<separator>", ",", "fl.pack~", ",", "fl.unpack~", ",", "fl.streamid~", ",", "<separator>", ",", "(Spectral)", ",", "<separator>", ",", "fl.convolve~", ",", "fl.fft~", ",", "fl.ifft~", ",", "fl.multitaper~", ",", "fl.window~", ",", "<separator>", ",", "(Storage)", ",", "<separator>", ",", "fl.recall~", ",", "fl.register~", ",", "fl.store~", ",", "<separator>", ",", "(Timing)", ",", "<separator>", ",", "fl.now~", ",", "fl.ticks~", ",", "fl.timer~", ",", "fl.timedelta~", ",", "<separator>", ",", "(Filters)", ",", "<separator>", ",", "fl.0dfsvf~", ",", "fl.onepolezero~", ",", "fl.onepole~", ",", "fl.resonant~", ",", "fl.sallenkey~", ",", "<separator>", ",", "(Spatial)", ",", "<separator>", ",", "fl.coordinatesystem~", ",", "fl.spatial", ",", "<separator>" ],
					"maxclass" : "umenu",
					"numinlets" : 1,
					"numoutlets" : 3,
					"outlettype" : [ "int", "", "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 6.0, 77.0, 142.0, 21.0 ],
					"style" : ""
				}

			}
, 			{
				"box" : 				{
					"fontface" : 3,
					"fontname" : "Arial",
					"fontsize" : 20.871338,
					"id" : "obj-1",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 10.0, 8.0, 485.0, 30.0 ],
					"style" : "",
					"text" : "Frame Lib Help",
					"textcolor" : [ 0.93, 0.93, 0.97, 1.0 ],
					"varname" : "autohelp_top_title"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 12.754705,
					"id" : "obj-2",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 10.0, 36.0, 485.0, 21.0 ],
					"style" : "",
					"textcolor" : [ 0.93, 0.93, 0.97, 1.0 ],
					"varname" : "autohelp_top_digest"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 11.595187,
					"id" : "obj-4",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 10.0, 57.0, 485.0, 19.0 ],
					"style" : "",
					"varname" : "autohelp_top_description"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"bgoncolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"fontface" : 3,
					"fontlink" : 1,
					"fontsize" : 12.754705,
					"id" : "obj-5",
					"maxclass" : "textbutton",
					"numinlets" : 1,
					"numoutlets" : 3,
					"outlettype" : [ "", "", "int" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 450.920898, 22.0, 49.079117, 14.249397 ],
					"presentation_rect" : [ 0.0, 0.0, 49.079117, 14.249397 ],
					"spacing_x" : 0.0,
					"spacing_y" : 0.0,
					"style" : "",
					"textcolor" : [ 0.361, 0.455, 0.596, 1.0 ],
					"textoncolor" : [ 0.27, 0.35, 0.47, 1.0 ],
					"textovercolor" : [ 0.4, 0.5, 0.65, 1.0 ],
					"underline" : 1,
					"varname" : "autohelp_top_ref"
				}

			}
, 			{
				"box" : 				{
					"angle" : 270.0,
					"background" : 1,
					"grad1" : [ 0.27, 0.35, 0.47, 1.0 ],
					"grad2" : [ 0.85, 0.85, 0.85, 1.0 ],
					"id" : "obj-6",
					"maxclass" : "panel",
					"mode" : 1,
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 5.0, 5.0, 495.0, 52.0 ],
					"proportion" : 0.39,
					"style" : "",
					"varname" : "autohelp_top_panel"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-3", 0 ],
					"source" : [ "obj-7", 1 ]
				}

			}
 ],
		"dependency_cache" : [  ],
		"autosave" : 0,
		"bgfillcolor_type" : "gradient",
		"bgfillcolor_color1" : [ 0.376471, 0.384314, 0.4, 1.0 ],
		"bgfillcolor_color2" : [ 0.290196, 0.309804, 0.301961, 1.0 ],
		"bgfillcolor_color" : [ 0.290196, 0.309804, 0.301961, 1.0 ],
		"bgfillcolor_angle" : 270.0,
		"bgfillcolor_proportion" : 0.39
	}

}
