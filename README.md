FrameLib
========

![FrameLib Logo](logo.png)

A DSP framework for arbitrary size frame processing with arbitrary sub-sample accurate timing.
* Frame timing and length can vary freely across networks and over time.
* Memory allocation is handled by the framework.
* FrameLib supports multi-threaded processing in real-time or non real-time.
* FrameLib can be compiled and bound to any environment that processes (or can process) audio in blocks.
* FrameLib can be easily extended.

FrameLib might be useful for:
---------------------------------
- Granular or spectral processing applications or more generally for multirate processing
- Working in cycling 74's Max with advanced spectral or multirate processing
- Prototyping or rapid development of frame-based DSP chains

FrameLib currently consists of:
---------------------------------
- A framework written in C++11 for frame-based processing
- A set of around 160 objects for processing frames
- A fully-functional binding and package for cycling 74's Max with documentation
- Proof-of-concept bindings for for puredata (pd) and SuperCollider (both work-in-progress)

Where to start:
---------------------------------
- To get the Max package: https://github.com/AlexHarker/FrameLib/releases/
- To compile FrameLib clone this repo and start in the XCode project or VS solution
- For max builds you will require https://github.com/Cycling74/max-sdk-base (to be placed alongside this repo in the same enclosing folder)
- If you wish to code new objects read the (limited) doxygen documentation of the framework source code
- FrameLib can be discussed at https://framelib.discourse.group

What is currently on the horizon:
---------------------------------
- Full SuperCollider support (there are some key issues to address listed alongside the SC work)

Credits
---------------------------------
By Alex Harker
CeReNeM - The University of Huddersfield

Max Documentation by James Bradbury.

With thanks to all the beta testers.
Special thanks to Owen Green, Francesco Cameli and James Bradbury for code for some of the objects.

**Contact:**

* ajharker@gmail.com
