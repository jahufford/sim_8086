#
# 'make'        build executable file 'main'
# 'make clean'  removes all .o and executable files
#
# define the Cpp compiler to use
CXX = g++
Target = $@
# define any compile-time flags
CXXFLAGS	:= -std=c++23 -Wall -Wextra -g

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS =

# define output directory
OUTPUT	:= bin

# define source directory
SRC		:= src
TESTS 	:= tests

# define include directory
INCLUDE	:= include

# define lib directory
LIB		:= lib

ifeq ($(OS),Windows_NT)
MAIN	:= main.exe
SOURCEDIRS	:= $(SRC)
INCLUDEDIRS	:= $(INCLUDE)
LIBDIRS		:= $(LIB)
FIXPATH = $(subst /,\,$1)
RM			:= del /q /f
MD	:= mkdir
else
MAIN	:= sim_8086
TEST	:= TEST_sim_8086
TESTDIRS	:= $(shell find $(SRC) -type d)
SOURCEDIRS	:= $(filter-out $(SRC)/$(TESTS)%, $(TESTDIRS))
INCLUDEDIRS	:= $(shell find $(INCLUDE) -type d)
LIBDIRS		:= $(shell find $(LIB) -type d) /usr/local/lib
FIXPATH = $1
RM = rm -f
MD	:= mkdir -p
endif

# define any directories containing header files other than /usr/include
INCLUDES	:= $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))

# define the C libs
LIBS		:= $(patsubst %,-L%, $(LIBDIRS:%/=%))
TESTLIBS    := $(patsubst %,-L%, $(LIBDIRS:%/=%)) -l\:libgtest.a -l\:libgtest_main.a

# define the C source files
# ALLSOURCES  := $(wildcard $(patsubst %,%/*.cpp, $(SOURCEDIRS)))
SOURCES  := $(wildcard $(patsubst %,%/*.cpp, $(SOURCEDIRS)))
#SOURCES		:= $(filter-out $(SRC)/test%, $(ALLSOURCES))
# ALLSOURCES  := $(wildcard $(patsubst %,%/*.cpp, $(SOURCEDIRS)))
# TESTSOURCES := $(filter-out $(SRC)/main.cpp, $(ALLSOURCES))
TESTSOURCES  := $(wildcard $(patsubst %,%/*.cpp, $(TESTDIRS)))
TESTSOURCES := $(filter-out $(SRC)/main.cpp, $(TESTSOURCES))

# define the C object files
OBJECTS		:= $(SOURCES:.cpp=.o)
TESTOBJECTS		:= $(TESTSOURCES:.cpp=.o)

# define the dependency output files
DEPS		:= $(OBJECTS:.o=.d)

#
# The following part of the makefile is generic; it can be used to
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

OUTPUTMAIN	:= $(call FIXPATH,$(OUTPUT)/$(MAIN))
TESTMAIN	:= $(call FIXPATH,$(OUTPUT)/$(TEST))

dummy:
	@echo $(TESTSOURCES)
	@echo $(SOURCES)

# all: $(OUTPUT) $(MAIN) $(TEST)
all: $(OUTPUT) $(MAIN) tests
	@echo Executing 'all' complete!

$(OUTPUT):
	$(MD) $(OUTPUT)

#NOTE: the main program has to be call main.cpp, or else you have to change it manually below for all targets
$(MAIN): $(OBJECTS)
#	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -MMD src/main.cpp  -o src/main.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(OUTPUTMAIN) $(OBJECTS) $(LFLAGS) $(LIBS)

tests: $(TESTOBJECTS)
# $(CXX) $(CXXFLAGS) -DTESTING $(INCLUDES) -c -MMD src/main.cpp  -o src/main.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TESTMAIN) $(TESTOBJECTS) $(LFLAGS) $(TESTLIBS) 
# include all .d files
# -include $(DEPS)

# this is a suffix replacement rule for building .o's and .d's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file)
# -MMD generates dependency output files same name as the .o file
# (see the gnu make manual section about automatic variables)
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -MMD $<  -o $@

.PHONY: clean
clean:
	$(RM) $(OUTPUTMAIN)
	$(RM) $(TESTMAIN)
	$(RM) $(call FIXPATH,$(OBJECTS))
	$(RM) $(call FIXPATH,$(DEPS))
	@echo Cleanup complete!

run: all
	./$(OUTPUTMAIN)
	@echo Executing 'run: all' complete!
