SHELL = /bin/sh

DUMP_MACHINE = $(shell cc -dumpmachine)
MACHINE_TYPE = $(word 2, $(subst -, , $(DUMP_MACHINE)))
PLATFORM_TYPE = $(MACHINE_TYPE)
 
# Detect windows

ifeq ($(MACHINE_TYPE), w64)
PLATFORM_TYPE = win
endif
ifeq ($(MACHINE_TYPE), w32)
PLATFORM_TYPE = win
endif

# Paths

FL_FRAMEWORK = ../FrameLib_Framework
FL_DEPENDENCIES = ../FrameLib_Dependencies
FL_OBJECTS = ../FrameLib_Objects

VPATH = ../FrameLib_Dependencies/tlsf

FL_CPP = \
				$(wildcard $(FL_FRAMEWORK)/FrameLib_*.cpp)\
				$(wildcard $(FL_OBJECTS)/*/FrameLib_*.cpp)\
				$(FL_DEPENDENCIES)/HISSTools_FFT/HISSTools_FFT.cpp

FL_OBJ_DIR = $(FL_CPP:.cpp=.o) tlsf.o
FL_OBJ = $(notdir $(FL_OBJ_DIR))

INCLUDE_PATHS = \
        -ICommon\
				-I$(FL_FRAMEWORK)\
			  -I$(FL_DEPENDENCIES)\
				-I$(FL_OBJECTS)/Common_Utilities/
	
MAIN_OBJ = framelib_pd.o

# Options

OPTIMISATION = -O3

CFLAGS = $(OPTIMISATION)
CPPFLAGS = -DNDEBUG -DFRAMELIB_RT_DSP_CHECK
CXXFLAGS = $(OPTIMISATION)

# Platform Options

ifeq ($(PLATFORM_TYPE), apple)
EXTENSION = d_fat
LINK_FLAGS = $(shell sed 's/\./-Wl,-U,/g' Common/pd_symbols.txt) -lstdc++ -lpthread -framework Accelerate 
PLATFORM_FLAGS = -bundle -flat_namespace -fPIC
endif

ifeq ($(PLATFORM_TYPE), linux)
CXXFLAGS += -Wno-ignored-attributes -Wno-stringop-overflow
EXTENSION = pd_linux
LINK_FLAGS = $(shell sed 's/\./-Wl,-u,/g' Common/pd_symbols.txt) -lm -lstdc++ -lpthread
PLATFORM_FLAGS = -no-pie -rdynamic -fPIC -shared
endif

ifeq ($(PLATFORM_TYPE), win)
CPPFLAGS += -D_USE_MATH_DEFINES -DMSW -DNT
CXXFLAGS += -Wno-ignored-attributes -Wno-stringop-overflow
EXTENSION = dll
LINK_FLAGS = $(shell sed 's/\._/-Wl,-ignore-unresolved-symbol,__imp_/g' Common/pd_symbols.txt) -lm -lstdc++ -lpthread
PLATFORM_FLAGS = -static-libgcc -static-libstdc++ -shared
endif

ifeq ($(MACHINE_TYPE), w64)
CPPFLAGS += -DPD_LONGINTTYPE=intptr_t
endif

OUTPUT_NAME = framelib_pd.$(EXTENSION)

# Targets and pattern rules

framelib_pd: $(MAIN_OBJ) $(FL_OBJ_DIR);
				cc $(CPPFLAGS) $(CXXFLAGS) $(PLATFORM_FLAGS) -o $(OUTPUT_NAME) $(MAIN_OBJ) $(FL_OBJ) $(LINK_FLAGS)

%.o : %.cpp
				g++ $(CPPFLAGS) $(CXXFLAGS) -std=c++11 -fPIC $(INCLUDE_PATHS) -c $<

%.o : %.c
				cc $(CPPFLAGS) $(CFLAGS) -fPIC $(INCLUDE_PATHS) -c $<
			
clean:
				rm $(MAIN_OBJ) $(FL_OBJ)
