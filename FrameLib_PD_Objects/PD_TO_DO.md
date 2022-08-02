
**Status and Building**

- Currently most framelib objects get built to a single library files
- Host communication object (fl.topad~/fl.frompd~) currently need to be added.
- Realtime operation is supported, but non-realtime support is incomplete  
- There are some key issues to note below 

To build you should be able to run make on this directory and then copy framelib_pd.d_fat and set it to load on startup

Building *should* work on linux (it works on apple) but I've not tested.
I'm not sure if there should be a VS project for windows or not.

**Testing**

- I've started a test folder at Testing/02_PD/ with at least one simple demo
- Not the first outlet and inlet of unsynced objects are the sync IO

**Key Issues**

- *Audio synchronisation* - this is the main issue with finishing pd support and the Max approach won't work There
- Because audio inputs and outputs are in different objects they must process audio in the correct order
- In Max this is achieved by making these objects own subpatches with the actual object inside and making invisible audio connections to force the ordering
- At the moment in pd you get "unsynced" objects and must make these connections manually
- Miller suggesting making all outputs signal outputs, which would work but isn't nice and is wasteful 
- My next step is to try to understand d_ugen.c better to see if there's another way

- Subpatches - my understanding is that pd processes the audio in subpatchers in one go (Max does not) which makes ordering an issue (as above)
- At the moment you can't connect framelib objects between different subpatchers

- Message ordering - If messages are send to pd then currently the message time and stream determines the order. In Max there are further considerations to do with object position in a patch so that ordering makes sense - I'm not sure what would be idomatic in pd.

- Attributes - non-realtime in Max is set by attributes. Part of the misssing support in pd is to sort these (which must be supported manually)
- My memory is that sigmund uses an attribute-style system so I want to choose something idiomatic to pd for setting these in the box.

- Multichannel buffers - my understanding is that pd buffers are mono only - buffers for framelib can be used both for reading and also for non-realtime operation, so figuring out how to support multichannel in an idomatic way would be good

**Documentation**

- Help files required (based on max help files) for each object
- Tutorials - consider if these should be translated somehow?
- Reference - I'm not sure if there is an equivalent to the reference system, or if the info message to framelib objects is sufficient

**Build System and Distribution**

- The makefile is very basic and currently builds 64 bit only 
- Improve the makefile or build system to support all necessary platforms and OSes
- At the moment object files go into the main PD folder - I'd like them to go into a build directory but I don't know make well enough
- At the moment I suppress undefined symbols at link, but that means that if there was a non-pd-related link error it wouldn't be caught, so if something nicer is possible that would be good.
