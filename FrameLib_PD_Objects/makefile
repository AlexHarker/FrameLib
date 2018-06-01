.SUFFIXES:.so
OBJECTBASE=../FrameLib_Objects

VPATH = PD_Specific:\ ../FrameLib_Dependencies/tlsf

FRAMELIBCPP=  \
$(wildcard ../FrameLib_Framework/FrameLib_*.cpp)\
$(wildcard ../FrameLib_Objects/*/FrameLib_*.cpp)\
../FrameLib_Dependencies/HISSTools_FFT/HISSTools_FFT.cpp\
fl.read_pd~.cpp\

FRAMELIBOBJ1=$(FRAMELIBCPP:.cpp=.o) tlsf.o
FRAMELIBOBJ=$(notdir $(FRAMELIBOBJ1))

FRAMELIBOBJINCLUDES=  \
$(patsubst %,-I%,$(sort $(dir $(wildcard ../FrameLib_Objects/*/))))

INCLUDEPATH=$(FRAMELIBOBJINCLUDES)\
        -IPD_Specific \
        -I../FrameLib_Dependencies \
        -I../FrameLib_Framework \
        -ICommon

OBJS = framelib_pd.o

OPTIM=-O3
#OPT=-g

framelib_pd.d_fat: framelib_pd.o $(FRAMELIBOBJ1); \
        cc $(OPTIM) -arch i386 -arch x86_64 -bundle -flat_namespace -undefined suppress -o framelib_pd.d_fat $(OBJS) \
        $(FRAMELIBOBJ) -lstdc++ -lpthread
	cp framelib_pd.d_fat /Users/alexharker/Documents/Pd/externals

.cpp.o:; g++ $(OPTIM) -arch i386 -arch x86_64 -std=c++11 -fPIC $(INCLUDEPATH) -c $<

.c.o:; cc $(OPTIM) -arch i386 -arch x86_64 -fPIC $(INCLUDEPATH) -c $<
