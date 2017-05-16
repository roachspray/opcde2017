# Point towwards the LLVM 4.0 installation (might work with 3.9)
LLBIN=/Users/jaredcarlson/Projects/llvm-4-build/bin
LLVM_CONFIG=$(LLBIN)/llvm-config
#QUIET:=@
QUIET:=


SRC_DIR?=$(PWD)/src
LDFLAGS+=$(shell $(LLVM_CONFIG) --ldflags)

COMMON_FLAGS=-Wall -Wextra -g


CXXFLAGS+=$(COMMON_FLAGS) $(shell $(LLVM_CONFIG) --cxxflags)
CPPFLAGS+=$(shell $(LLVM_CONFIG) --cppflags) -std=c++11 -I$(SRC_DIR) 

ifeq ($(shell uname),Darwin)
LOADABLE_MODULE_OPTIONS=-bundle -undefined dynamic_lookup
PASS=TestFunctionPass.dylib
else
LOADABLE_MODULE_OPTIONS=-shared -Wl,-O1
PASS=TestFunctionPass.so
endif

# Change these to point to your installation of clang/llvm-d
LDIS=$(LLBIN)/llvm-dis
CPP=/usr/bin/clang++
CC=/usr/bin/clang

BOD=build/obj
PASSMGR=Manager
OPM=build/bin/$(PASSMGR)

PASS_OBJECTS=Analysis/TestFunctionPass.o 

# XXX
# This is awful... Im just like "PUT IT ALL IN"
LIBS=$(shell $(LLVM_CONFIG) --libs) -lclang
LIBS+=-lpthread -ldl -lncurses -lz

TDIR=build/tests

default: prep $(PASS) passmgr

prep:
	@echo "Prep phase"
	$(QUIET)mkdir -p build
	$(QUIET)mkdir -p build/obj
	$(QUIET)mkdir -p build/obj/Analysis
	$(QUIET)mkdir -p build/bin
	$(QUIET)mkdir -p build/lib

define builditdood
$(QUIET)$(CPP) -o $(BOD)/$(1)/$(@F) -c $(CPPFLAGS) $(CXXFLAGS) $<
endef

Analysis/%.o: $(SRC_DIR)/Analysis/%.cpp
	@echo "Compiling $*.cpp"
	$(call builditdood,Analysis)

%.o : $(SRC_DIR)/%.cpp
	@echo "Compiling $*.cpp"
	$(call builditdood,.)

passmgr:
	@echo "Building passmanager clean up ldflags XXX"
	$(QUIET)$(CPP) -o $(BOD)/manager.o -c $(CPPFLAGS) $(CXXFLAGS) src/manager.cpp
	$(QUIET)$(CPP) -o $(OPM) $(CXXFLAGS) build/obj/manager.o ${addprefix $(BOD)/,$(PASS_OBJECTS)} $(LDFLAGS) $(LIBS)


$(PASS) : $(PASS_OBJECTS)
	@echo "Linking $@"
	$(QUIET)$(CPP) -o build/lib/$@ $(LOADABLE_MODULE_OPTIONS) $(CXXFLAGS) $(LDFLAGS) ${addprefix $(BOD)/,$^}

test: testprep testfe

testprep:
	$(QUIET)mkdir -p $(TDIR)

testfe:
	$(QUIET)$(CC) -o $(TDIR)/FE001 tests/FE001.c
	$(QUIET)$(CC) -g -emit-llvm -o $(TDIR)/FE001.bc -c tests/FE001.c
	$(QUIET)$(LDIS) $(TDIR)/FE001.bc

help:
	@echo "make "
	@echo "...See build/"
	@echo "make clean or make cleanall which requires jsoncpp rebuild"
	@echo "make test"
	@echo "make runtests"

runtests: runfe

runfe:
	@echo "***"
	@echo "***   Running: Function Pass Manager with Function Recogition ***"
	@echo "***"
	$(QUIET)$(OPM) -function-recognition $(TDIR)/FE001.bc $(TDIR)/FE001_out.bc

clean:
	$(QUIET)rm -rf build tests/*.ll

cleanall: clean

