.SUFFIXES:.so
FL_FRAMEWORK=../FrameLib_Framework
FL_DEPENDENCIES=../FrameLib_Dependencies
FL_OBJECTS=../FrameLib_Objects

VPATH=PD_Specific:\ ../FrameLib_Dependencies/tlsf

FL_CPP= \
				$(wildcard $(FL_FRAMEWORK)/FrameLib_*.cpp)\
				$(wildcard $(FL_OBJECTS)/*/FrameLib_*.cpp)\
				$(FL_DEPENDENCIES)/HISSTools_FFT/HISSTools_FFT.cpp

FL_OBJ_DIR=$(FL_CPP:.cpp=.o) tlsf.o
FL_OBJ=$(notdir $(FL_OBJ_DIR))

INCLUDEPATH=\
				-IPD_Specific\
        -ICommon\
				-I$(FL_FRAMEWORK)\
			  -I$(FL_DEPENDENCIES)\
				-I$(OBJECTBASE)/Common_Utilities/
	
MAIN_OBJ=framelib_pd.o
OPTIMISATION=-O3
#OPT=-g

DUMPMACHINE=$(shell cc -dumpmachine)
MACHINETYPE=$(word 2, $(subst -, , $(DUMPMACHINE)))

framelib_pd.d_fat: $(MAIN_OBJ) $(FL_OBJ_DIR); 
ifeq ($(MACHINETYPE), apple)
				cc $(OPTIMISATION) -arch x86_64 -bundle -flat_namespace -undefined suppress -o framelib_pd.d_fat $(MAIN_OBJ) \
				$(FL_OBJ) -lstdc++ -lpthread -framework Accelerate 
else 
				cc $(OPTIMISATION) -arch x86_64 -bundle -flat_namespace -undefined suppress -o framelib_pd.d_fat $(MAIN_OBJ) \
				$(FL_OBJ) -lstdc++ -lpthread
endif

%.o : %.cpp
				g++ $(OPTIMISATION) -arch x86_64 -std=c++11 -fPIC $(INCLUDEPATH) -c $<

%.o : %.c
				cc $(OPTIMISATION) -arch x86_64 -fPIC $(INCLUDEPATH) -c $<
