FrameLib
========

![FrameLib Logo](logo.png)

A DSP framework for arbitrary size frame processing with arbitrary sub-sample accurate timing.
* Frame timing and length can vary freely across networks and over time.
* Memory allocation is handled by the framework.
* FrameLib supports multi-threaded processing in real-time or non-realtime.
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
- To get the Max package download it from the Package Manager via the Max application 
- To compile FrameLib clone this repo and start in the XCode project or VS solution
- For max builds you will require https://github.com/Cycling74/max-sdk-base (to be placed alongside this repo in the same enclosing folder)
- If you wish to code new objects read the (limited) doxygen documentation of the framework source code
- FrameLib can be discussed at https://framelib.discourse.group

Project generator templates:
---------------------------------
- To override an object-specific source entry, place a template named `<class_file_without_FrameLib_>_<template>` next to the corresponding default template in `Scripts/project_tools/base_templates` (for example, `YINFFT_file_object`). Entries without an override use the default template.
- If `Config_<FrameLib_class_file>_Max.xcconfig` exists in the repository root, Xcode project generation adds its file reference if necessary and uses the generic `object_specific_build_config_*` templates for that object's build configurations. Removal uses the cached configuration GUID even if the file has been deleted, and removes the Xcode file reference when no build configurations use it. The config file itself is not deleted. If the file has been deleted, the cached GUID is cleared so subsequent rebuilds use the default templates.

Dependencies:
---------------------------------
- Initialise the repository dependencies with `git submodule update --init --recursive`.
- The `fl.yinfft~` object uses FluCoMa core, Eigen and foonathan memory. For Xcode or Visual Studio builds, install CMake and run `python3 Scripts/setup_flucoma.py` from the repository root after initialising the submodules. On Windows, use `python Scripts/setup_flucoma.py --platform Win32` for 32-bit builds (the default is `x64`). Run the setup again if the platform or compiler changes. The PD makefile builds the dependency automatically.
- In Xcode, the standalone `fl.yinfft~` target links foonathan memory directly, while the `framelib_objects` static library incorporates it for the single-binary Max, PD, SuperCollider and other object-library consumers.
- In Visual Studio, `framelib_objects.lib` incorporates the configuration-matched foonathan memory library; individual Max projects do not link it separately.
- `fl.yinfft~` accepts a non-negative magnitude spectrum including DC and Nyquist, with `N / 2 + 1` bins for a power-of-two FFT of length `N`. It outputs pitch in Hz and confidence as separate single-value frames. The `max_length` instantiation parameter sets the maximum FFT size; `min_frequency` and `max_frequency` set the pitch search bounds. Bounds must be positive, ordered and no higher than Nyquist. Silent spectra yield zero for both outputs.
- FluCoMa core is distributed under the BSD 3-Clause license, Eigen under MPL2 (and other licenses for portions not used here), and foonathan memory under the zlib license. Include their license notices when distributing binaries.
- After building the Xcode `framelib_objects` target, the focused YINFFT test can be run on macOS with:
  `clang++ -std=c++11 -IFrameLib_Framework Testing/YINFFTTests.cpp build/Development/libframelib_objects.a build/Development/libframelib.a -framework Accelerate -o /tmp/framelib-yinfft-tests && /tmp/framelib-yinfft-tests`

What is currently on the horizon:
---------------------------------
- Full SuperCollider support (there are some key issues to address listed alongside the SC work)

FrameLib makes use of:
---------------------------------

- **tlsf** - the two level segregated fit memory allocator by *Matt Conte*: <br>
https://github.com/mattconte/tlsf

- **convhull_3d** - by *Leo McCormack*: <br> 
https://github.com/leomccormack/convhull_3d

Copright and licenses for these portions of the software are included with the relevant source files.

Credits
---------------------------------
**FrameLib** By *Alex Harker* <br>
CeReNeM - The University of Huddersfield

Max Documentation by *James Bradbury*.

With thanks to all the beta testers.

Special thanks and credit to *Owen Green*, *Francesco Cameli* and *James Bradbury* for code contributions for FrameLib objects.

Contact:
---------------------------------

* ajharker@gmail.com
