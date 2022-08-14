
**Status / Building / Testing**

- All (165 out of 165) framelib objects get built to a single library files
- Realtime operation and non-realtime operation is supported

To build you should be able to run make on this directory and then copy /build/framelib_pd.* and set it to load on startup

- I've started a test folder at Testing/02\_PD with a few simple demos

**Key Issues**

- Subpatches - my understanding is that pd processes the audio in subpatchers in one go (Max does not) which makes ordering an issue (as above)
- At the moment you can't connect framelib objects between different subpatchers

- Multichannel buffers - my understanding is that pd buffers are mono only - buffers for framelib can be used both for reading and also for non-realtime operation, so figuring out how to support multichannel in an idomatic way would be good

- *Audio synchronisation* - this is the main issue with finishing pd support and the Max approach won't work here
- Because audio inputs and outputs are in different objects they must process audio in the correct order
- In Max this is achieved by making these objects own subpatches with the actual object inside and making invisible audio connections to force the ordering
- At the moment in pd you get "unsynced" objects and must make these connections manually
- Miller suggesting making all outputs signal outputs, which would work but isn't nice and is wasteful 
- My next step is to try to understand d_ugen.c to understand how the dsp graph is built and if there is a better way
- Having looked at this there doesn't appear to be any way other than using signal IO for everything so I've implemented that [REVIEW]

- Connection handling - Max has the facility both to notify objects when connection change and allow objects to refuse connections - this is used to make connections prior to dsp time and also to prevent multiple connections - I have left this out and not yet investigated if pd has something similar (it's non-essential but useful). There's no mechanism for this in pd so work needs to be done to make sure errors report correctly only, which appears covered by the pre-existing code [REVIEW]

- Attributes - non-realtime in Max is set by attributes. Part of the misssing support in pd is to sort these (which must be supported manually)
- My memory is that sigmund uses an attribute-style system so I want to choose something idiomatic to pd for setting these in the box.
- sigmund~ uses dashes like the command line so I've copied that for now. [REVIEW]

- Message Ordering - If messages are sent to pd then currently the message time and stream determines the order. In Max there are further considerations to do with object position in a patch so that ordering makes sense - I'm not sure what would be idomatic in pd but logically patch position would seem to make sense, so I'm implenting that [REVIEW]

**Review**

- Some pd correctness has been reviewed (DONE):
    - Float arguments (t_floatarg?)
    - Assist strings? (don't exist for pd)
    - Messages that would take ints in Max
    - resetting dsp when resolving connections

- Some pd correctness needs review:
    - PD-specific or custom objects (read/info/topd/frompd/contextcontrol/expressions)
    - Whether garray_usedindsp should be used
    
**Documentation**

- Help files required (based on max help files) for each object
- Tutorials - consider if these should be translated somehow?
- Reference - I'm not sure if there is an equivalent to the reference system, or if the info message to framelib objects is sufficient

**Build System and Distribution**

- I need to understand what file names/architectures etc. should be for different systems 
- Building currently works with CI on apple (arm / x86_64), linux (i386 / x86_64) and windows (x86_64)
