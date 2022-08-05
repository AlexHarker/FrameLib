DUMP_MACHINE=$(shell cc -dumpmachine)
MACHINE_TYPE=$(word 2, $(subst -, , $(DUMP_MACHINE)))

FL_FRAMEWORK=../FrameLib_Framework
FL_DEPENDENCIES=../FrameLib_Dependencies
FL_OBJECTS=../FrameLib_Objects

VPATH=../FrameLib_Dependencies/tlsf

FL_CPP= \
				$(wildcard $(FL_FRAMEWORK)/FrameLib_*.cpp)\
				$(wildcard $(FL_OBJECTS)/*/FrameLib_*.cpp)\
				$(FL_DEPENDENCIES)/HISSTools_FFT/HISSTools_FFT.cpp

FL_OBJ_DIR=$(FL_CPP:.cpp=.o) tlsf.o
FL_OBJ=$(notdir $(FL_OBJ_DIR))

INCLUDE_PATHS=\
        -ICommon\
				-I$(FL_FRAMEWORK)\
			  -I$(FL_DEPENDENCIES)\
				-I$(FL_OBJECTS)/Common_Utilities/
	
MAIN_OBJ=framelib_pd.o
OPTIMISATION=-O3

CFLAGS+=$(OPTIMISATION)
CPPFLAGS+=-DNDEBUG -DFRAMELIB_RT_DSP_CHECK

ifeq ($(MACHINE_TYPE), apple)
CXXFLAGS+=$(OPTIMISATION) _USE_MATH_DEFINES
EXTENSION=d_fat
LINK_FLAGS=$(shell sed 's/-u/-U/' Common/pd_linker_flags.txt | tr '\n' ' ') -lstdc++ -lpthread -framework Accelerate 
PLATFORM_FLAGS=-bundle -flat_namespace -fPIC
endif

ifeq ($(MACHINE_TYPE), linux)
CXXFLAGS+=$(OPTIMISATION) -Wno-ignored-attributes -Wno-stringop-overflow
EXTENSION=pd_linux
LINK_FLAGS=$(shell tr '\n' ' ' < Common/pd_linker_flags.txt) -lm -lstdc++ -lpthread
PLATFORM_FLAGS=-no-pie -rdynamic -fPIC -shared
endif

ifeq ($(MACHINE_TYPE), win)
CPPFLAGS+=-D_USE_MATH_DEFINES
CXXFLAGS+=$(OPTIMISATION) -Wno-ignored-attributes
EXTENSION=dll
LINK_FLAGS=$(shell tr '\n' ' ' < Common/pd_linker_flags.txt) -lm -lstdc++ -lpthread
PLATFORM_FLAGS=-static-libgcc -static-libstdc++ -shared
endif

OUTPUT_NAME=framelib_pd.$(EXTENSION)
#OPT=-g

framelib_pd: $(MAIN_OBJ) $(FL_OBJ_DIR); \
cc $(CPPFLAGS) $(CXXFLAGS) $(PLATFORM_FLAGS) -o $(OUTPUT_NAME) $(MAIN_OBJ) $(FL_OBJ) $(LINK_FLAGS)

%.o : %.cpp
				g++ $(CPPFLAGS) $(CXXFLAGS) -std=c++11 -fPIC $(INCLUDE_PATHS) -c $<

%.o : %.c
				cc $(CPPFLAGS) $(CFLAGS) -fPIC $(INCLUDE_PATHS) -c $<
			
clean:
				rm $(MAIN_OBJ) $(FL_OBJ)
