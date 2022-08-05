.SUFFIXES:.so
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
CXXFLAGS+=$(OPTIMISATION)
CPPFLAGS+=-DNDEBUG -DFRAMELIB_RT_DSP_CHECK

LINK_FLAGS=$(shell cat Common/pd_linker_flags.txt)
#OPT=-g

DUMP_MACHINE=$(shell cc -dumpmachine)
MACHINE_TYPE=$(word 2, $(subst -, , $(DUMP_MACHINE)))

framelib_pd.d_fat: $(MAIN_OBJ) $(FL_OBJ_DIR); 
ifeq ($(MACHINE_TYPE), apple)
				cc $(CPPFLAGS) $(CXXFLAGS) -bundle -flat_namespace -o framelib_pd.d_fat $(MAIN_OBJ) \
				$(FL_OBJ) $(LINK_FLAGS) -lstdc++ -lpthread -framework Accelerate 
else 
				cc $(CPPFLAGS) $(CXXFLAGS) -o framelib_pd.d_fat $(MAIN_OBJ) \
				$(FL_OBJ) $(LINK_FLAGS) -lm -lstdc++ -lpthread
endif

%.o : %.cpp
				g++ $(CPPFLAGS) $(CXXFLAGS) -std=c++11 -fPIC $(INCLUDE_PATHS) -c $<

%.o : %.c
				cc $(CPPFLAGS) $(CFLAGS) -fPIC $(INCLUDE_PATHS) -c $<
			
clean:
				rm $(MAIN_OBJ) $(FL_OBJ)
