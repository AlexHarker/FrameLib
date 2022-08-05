.SUFFIXES:.so
	
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

ifeq ($(MACHINE_TYPE), linux)
CXXFLAGS+=$(OPTIMISATION) -Wno-ignored-attributes -Wno-stringop-overflow
else
CXXFLAGS+=$(OPTIMISATION)
endif

CFLAGS+=$(OPTIMISATION)
CPPFLAGS+=-DNDEBUG -DFRAMELIB_RT_DSP_CHECK

ifeq ($(MACHINE_TYPE), apple)
LINK_FLAGS=$(shell sed 's/-u/-U/' Common/pd_linker_flags.txt | tr '\n' ' ')
else
LINK_FLAGS=$(shell tr '\n' ' ' < Common/pd_linker_flags.txt)
endif

#OPT=-g

framelib_pd.d_fat: $(MAIN_OBJ) $(FL_OBJ_DIR); 
ifeq ($(MACHINE_TYPE), apple)
				cc $(CPPFLAGS) $(CXXFLAGS) -bundle -flat_namespace -fPIC -o framelib_pd.d_fat $(MAIN_OBJ) \
				$(FL_OBJ) $(LINK_FLAGS) -lstdc++ -lpthread -framework Accelerate 
else 
				cc $(CPPFLAGS) $(CXXFLAGS) -no-pie -rdynamic -fPIC -shared -o framelib_pd.d_fat $(MAIN_OBJ) \
				$(FL_OBJ) $(LINK_FLAGS) -lm -lstdc++ -lpthread
endif

%.o : %.cpp
				g++ $(CPPFLAGS) $(CXXFLAGS) -std=c++11 -fPIC $(INCLUDE_PATHS) -c $<

%.o : %.c
				cc $(CPPFLAGS) $(CFLAGS) -fPIC $(INCLUDE_PATHS) -c $<
			
clean:
				rm $(MAIN_OBJ) $(FL_OBJ)
