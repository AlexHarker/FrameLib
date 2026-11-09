SHELL = /bin/sh

DUMP_MACHINE = $(shell cc -dumpmachine)
ARCH_TYPE = $(word 1, $(subst -, , $(DUMP_MACHINE)))
MACHINE_TYPE = $(word 2, $(subst -, , $(DUMP_MACHINE)))
PLATFORM_TYPE = $(MACHINE_TYPE)
 
# Detect windows

ifeq ($(MACHINE_TYPE), pc)
PLATFORM_TYPE = win
BUILD_TYPE ?= x86_64
endif
ifeq ($(MACHINE_TYPE), w64)
PLATFORM_TYPE = win
BUILD_TYPE ?= x86_64
endif
ifeq ($(MACHINE_TYPE), w32)
PLATFORM_TYPE = win
BUILD_TYPE ?= i386
endif

BUILD_TYPE ?= $(ARCH_TYPE)
BUILD_DIR ?= ../build
INTERMEDIATE_DIR ?= $(BUILD_DIR)/objects
NAME_SUFFIX ?= $(BUILD_TYPE)

# Paths

FL_FRAMEWORK = ../FrameLib_Framework
FL_DEPENDENCIES = ../FrameLib_Dependencies
FL_OBJECTS = ../FrameLib_Objects

VPATH = ../FrameLib_Dependencies/tlsf

FL_CPP = \
				$(wildcard $(FL_FRAMEWORK)/FrameLib_*.cpp)\
				$(wildcard $(FL_OBJECTS)/*/FrameLib_*.cpp)\

FL_OBJ = tlsf.o $(FL_CPP:.cpp=.o) 
FL_OBJ_OUT = $(addprefix $(INTERMEDIATE_DIR)/, $(notdir $(FL_OBJ)))

INCLUDE_PATHS = \
				-I$(FL_FRAMEWORK)\
				-I$(FL_OBJECTS)/Common_Utilities/

FLUCOMA_BUILD = $(BUILD_DIR)/flucoma/$(NAME_SUFFIX)
FLUCOMA_LIBRARY = $(FLUCOMA_BUILD)/src/libfoonathan_memory-0.7.4.a
FLUCOMA_INCLUDES = -I$(FL_DEPENDENCIES)/flucoma-core/include -I$(FL_DEPENDENCIES)/eigen \
				-I$(FL_DEPENDENCIES)/HISSTools_Library/include \
				-I$(FL_DEPENDENCIES)/foonathan-memory/include \
				-I$(FL_DEPENDENCIES)/foonathan-memory/include/foonathan -I$(FLUCOMA_BUILD)/src
	
MAIN_OBJ = framelib_pd.o
MAIN_OBJ_OUT = $(INTERMEDIATE_DIR)/$(MAIN_OBJ)

# Options

OPTIMISATION = -O3

CFLAGS = $(OPTIMISATION)
CPPFLAGS = -DNDEBUG -DFRAMELIB_RT_DSP_CHECK
CXXFLAGS = $(OPTIMISATION)

# Platform Options

PLATFORM_FLAGS = 

ifeq ($(PLATFORM_TYPE), apple)
EXTENSION = d_fat
ARCH_FLAGS = -arch arm64 -arch x86_64
LINK_FLAGS = $(shell sed 's/\./-Wl,-U,/g' Common/pd_symbols.txt) -lstdc++ -lpthread -framework Accelerate 
PLATFORM_LINK_FLAGS = -bundle -flat_namespace -fPIC
endif

ifeq ($(PLATFORM_TYPE), linux)
CXXFLAGS += -Wno-ignored-attributes -Wno-stringop-overflow
EXTENSION = pd_linux
LINK_FLAGS = $(shell sed 's/\./-Wl,-u,/g' Common/pd_symbols.txt) -lm -lstdc++ -lpthread -latomic
PLATFORM_LINK_FLAGS = -no-pie -rdynamic -fPIC -shared
ARCH_FLAGS = 
ifeq ($(BUILD_TYPE), i386)
ARCH_FLAGS = -m32 -march=i386
endif
ifeq ($(ARCH_TYPE), arm)
ARCH_FLAGS = -mfpu=neon
endif
endif

ifeq ($(PLATFORM_TYPE), win)
CPPFLAGS += -D_USE_MATH_DEFINES -DMSW -DNT -DPD_LONGINTTYPE=intptr_t
CXXFLAGS += -Wno-ignored-attributes -Wno-stringop-overflow
EXTENSION = dll
LINK_FLAGS = -lm -lstdc++ -latomic
DLL = ../../pd-0.52-2/bin/pd.dll
PLATFORM_LINK_FLAGS = -static -static-libgcc -static-libstdc++ -Wl,--enable-auto-import "$(DLL)" -shared
PLATFORM_FLAGS = -Wa,-mbig-obj
ARCH_FLAGS = 
ifeq ($(BUILD_TYPE), i386)
DLL = ../../pd-0.52-2-i386/bin/pd.dll
ARCH_FLAGS = -m32 -march=i386
endif
endif

OUTPUT_NAME = $(BUILD_DIR)/framelib_pd-$(NAME_SUFFIX).$(EXTENSION)

# Targets and pattern rules

framelib_pd: build_directory flucoma_dependency $(FL_OBJ) $(MAIN_OBJ);
				cc $(CPPFLAGS) $(CXXFLAGS) $(ARCH_FLAGS) $(PLATFORM_LINK_FLAGS) -o $(OUTPUT_NAME) $(FL_OBJ_OUT) $(MAIN_OBJ_OUT) $(FLUCOMA_LIBRARY) $(LINK_FLAGS)

flucoma_dependency:
				cmake -S $(FL_DEPENDENCIES)/foonathan-memory -B $(FLUCOMA_BUILD) -DCMAKE_BUILD_TYPE=Release -DFOONATHAN_MEMORY_BUILD_TESTS=OFF -DFOONATHAN_MEMORY_BUILD_EXAMPLES=OFF -DFOONATHAN_MEMORY_BUILD_TOOLS=OFF $(FLUCOMA_CMAKE_FLAGS)
				cmake --build $(FLUCOMA_BUILD) --config Release

ifeq ($(PLATFORM_TYPE), apple)
FLUCOMA_CMAKE_FLAGS = -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.9
endif

$(FL_OBJECTS)/Spectral/FrameLib_YINFFT.o: | flucoma_dependency
$(FL_OBJECTS)/Spectral/FrameLib_YINFFT.o: $(FL_OBJECTS)/Spectral/FrameLib_YINFFT.cpp intermediate_directory
				g++ $(CPPFLAGS) $(CXXFLAGS) $(ARCH_FLAGS) $(PLATFORM_FLAGS) -std=c++17 -fPIC $(INCLUDE_PATHS) $(FLUCOMA_INCLUDES) -c $< -o $(INTERMEDIATE_DIR)/$(@F)

build_directory:
				mkdir -p $(BUILD_DIR)
				
intermediate_directory:
				mkdir -p $(INTERMEDIATE_DIR)
	
%.o : %.cpp intermediate_directory
				g++ $(CPPFLAGS) $(CXXFLAGS) $(ARCH_FLAGS) $(PLATFORM_FLAGS) -std=c++11 -fPIC $(INCLUDE_PATHS) -c $< -o $(INTERMEDIATE_DIR)/$(@F)

%.o : %.c intermediate_directory
				cc $(CPPFLAGS) $(CFLAGS) $(ARCH_FLAGS) $(PLATFORM_FLAGS) -fPIC $(INCLUDE_PATHS) -c $< -o $(INTERMEDIATE_DIR)/$(@F)
		
.PHONY: clean flucoma_dependency
		
clean:
				rm -f -r $(INTERMEDIATE_DIR) 
				rm -f $(OUTPUT_NAME) 
			
	
