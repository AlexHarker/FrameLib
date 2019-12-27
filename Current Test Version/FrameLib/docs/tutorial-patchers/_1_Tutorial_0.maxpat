{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 8,
			"minor" : 1,
			"revision" : 0,
			"architecture" : "x64",
			"modernui" : 1
		}
,
		"classnamespace" : "box",
		"rect" : [ 34.0, 79.0, 652.0, 787.0 ],
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
		"subpatcher_template" : "nobars",
		"showontab" : 1,
		"boxes" : [ 			{
				"box" : 				{
					"id" : "obj-17",
					"linecount" : 21,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 551.75, 213.0, 539.25, 288.0 ],
					"text" : "This tutorial is going to begin at an extremely basic and simple point of entry ino the world of frame-based processing and more general digital signal processing. If you feel at any point that the information here isn't useful or that you know these principles already then you are encouraged to move directly to the next tutorial.\n\nAudio in the real world is a continuous rarefaction and compression of a physical medium. Speakers\n\nA buffer of audio, some numbers (samples). \n\nThese samples represent the movements of the speakers that will shift the air back and forth. Depending on how these numbers are processed the resulting audio will change.\n\nNow, these samples are held in memory on your computer as a buffer~ in Max. We can read the samples back this buffer\n\nThe samples have to be converted into electric signals at exactly the right time, in order for the sound to be accurately reproduced. The computer deals with the complexity of this by creating a continuous strema of samples, by operating on small chunks, blocks, or vectors of audio at a consistent rate. There are reasons beyond the application of music why a computer would deal with groups of samples rather than each one at a time but that is outside the scope of what FrameLib is or the problems that it is attempting to solve."
				}

			}
, 			{
				"box" : 				{
					"border" : 2.0,
					"id" : "obj-7",
					"linecolor" : [ 0.65098, 0.666667, 0.662745, 1.0 ],
					"maxclass" : "live.line",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 572.75, 97.0, 530.25, 5.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"fontsize" : 20.0,
					"id" : "obj-12",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 572.75, 66.0, 260.0, 29.0 ],
					"text" : "A unique example"
				}

			}
, 			{
				"box" : 				{
					"border" : 2.0,
					"id" : "obj-6",
					"linecolor" : [ 0.65098, 0.666667, 0.662745, 1.0 ],
					"maxclass" : "live.line",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 5.75, 404.5, 530.25, 5.0 ]
				}

			}
, 			{
				"box" : 				{
					"border" : 2.0,
					"id" : "obj-5",
					"linecolor" : [ 0.65098, 0.666667, 0.662745, 1.0 ],
					"maxclass" : "live.line",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 5.75, 206.0, 530.25, 5.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Lato",
					"fontsize" : 44.0,
					"id" : "obj-1",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 5.75, 6.0, 197.0, 59.0 ],
					"text" : "FrameLib"
				}

			}
, 			{
				"box" : 				{
					"border" : 2.0,
					"id" : "obj-49",
					"linecolor" : [ 0.65098, 0.666667, 0.662745, 1.0 ],
					"maxclass" : "live.line",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 5.75, 97.0, 530.25, 5.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"fontsize" : 20.0,
					"id" : "obj-43",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 5.75, 66.0, 205.0, 29.0 ],
					"text" : "Start"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 13.0,
					"id" : "obj-14",
					"linecount" : 15,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 5.75, 411.5, 495.0, 224.0 ],
					"text" : "We traditionally conceive of digital sound as a continuous stream. This constantly 'flowing' signal can be directed to different objects that process the stream to achieve different sonic affects or control routines. This representation is somewhat deceptive and under the hood. The computer doesn't conceive of audio as a continuous signal, rather as contiguous blocks, or vectors that represent the audio stream. The size of these blocks is determined by your 'signal vector size' in your audio settings.\n\nFrameLib operates on a very similar premise and allows you to break down a 'continuous stream' into chunks that can be independently operated on. This is useful for processes where a discrete segment of audio is of significance - such as in granular synthesis (the grain) or in spectral processing via FFT (a spectral slice or window). While FrameLib requires you as the user to switch your thinking a little bit and to learn some new ways of working, it is incredibly powerful once you get to know the objects and how they interact with the normal Max world you are used to."
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"fontsize" : 20.0,
					"id" : "obj-11",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 5.75, 371.5, 260.0, 29.0 ],
					"text" : "Why is this important?"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 13.0,
					"id" : "obj-3",
					"linecount" : 4,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 5.75, 104.0, 539.0, 65.0 ],
					"text" : "Thanks for downloading and installing FrameLib! You might be wondering what a \"frame\" is, or perhaps you're an expert and want to jump right ahead. In any case, these tutorials will build you up from a point of almost zero knowledge about the concept and use of frame based processing with FrameLib."
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 13.0,
					"id" : "obj-4",
					"linecount" : 10,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 5.75, 213.0, 527.0, 152.0 ],
					"text" : "A frame is a list, vector or array of samples. All of these words refer to a similar idea, so put simply its just a group of numbers. The numbers can represent a variety of different things such as audio samples, buffer samples to read, spectral analysis information or gestural control data. FrameLib is a framework for generating and processing these kinds of information in a concise and streamlined manner. It is incredibly precise in time and the family of objects that comprise FrameLib can be integrated with Max objects seemlessly. FrameLib works entirely in the audio domain and has a specific set of objects that allow interaction between fl.objects~ and everything else. As a result it is highly precise (down to 2^64 of a sample), and operates on a system of 'scheduling' frames in advance with 'schedulers'. These can be thought of as that is similar to a metro object"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"fontsize" : 20.0,
					"id" : "obj-2",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 5.75, 175.0, 257.0, 29.0 ],
					"text" : "What is a frame?"
				}

			}
 ],
		"lines" : [  ],
		"dependency_cache" : [  ],
		"autosave" : 0,
		"styles" : [ 			{
				"name" : "AudioStatus_Menu",
				"default" : 				{
					"bgfillcolor" : 					{
						"type" : "color",
						"color" : [ 0.294118, 0.313726, 0.337255, 1 ],
						"color1" : [ 0.454902, 0.462745, 0.482353, 0.0 ],
						"color2" : [ 0.290196, 0.309804, 0.301961, 1.0 ],
						"angle" : 270.0,
						"proportion" : 0.39,
						"autogradient" : 0
					}

				}
,
				"parentstyle" : "",
				"multi" : 0
			}
, 			{
				"name" : "newobjBlue-1",
				"default" : 				{
					"accentcolor" : [ 0.317647, 0.654902, 0.976471, 1.0 ]
				}
,
				"parentstyle" : "",
				"multi" : 0
			}
, 			{
				"name" : "newobjBrown-1",
				"default" : 				{
					"accentcolor" : [ 0.654902, 0.572549, 0.376471, 1.0 ]
				}
,
				"parentstyle" : "",
				"multi" : 0
			}
, 			{
				"name" : "newobjGreen-1",
				"default" : 				{
					"accentcolor" : [ 0.0, 0.533333, 0.168627, 1.0 ]
				}
,
				"parentstyle" : "",
				"multi" : 0
			}
, 			{
				"name" : "newobjRed-1",
				"default" : 				{
					"accentcolor" : [ 0.784314, 0.145098, 0.023529, 1.0 ]
				}
,
				"parentstyle" : "",
				"multi" : 0
			}
, 			{
				"name" : "newobjYellow-1",
				"default" : 				{
					"fontsize" : [ 12.059008 ],
					"accentcolor" : [ 0.82517, 0.78181, 0.059545, 1.0 ]
				}
,
				"parentstyle" : "",
				"multi" : 0
			}
, 			{
				"name" : "numberGold-1",
				"default" : 				{
					"accentcolor" : [ 0.764706, 0.592157, 0.101961, 1.0 ]
				}
,
				"parentstyle" : "",
				"multi" : 0
			}
, 			{
				"name" : "panelViolet",
				"default" : 				{
					"bgfillcolor" : 					{
						"type" : "color",
						"color" : [ 0.372549, 0.196078, 0.486275, 0.2 ],
						"color1" : [ 0.454902, 0.462745, 0.482353, 1.0 ],
						"color2" : [ 0.290196, 0.309804, 0.301961, 1.0 ],
						"angle" : 270.0,
						"proportion" : 0.39,
						"autogradient" : 0
					}

				}
,
				"parentstyle" : "",
				"multi" : 0
			}
 ]
	}

}
