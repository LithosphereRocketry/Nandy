# Semi-automated C/C++ makefile

# This should be all you need to change
TARGETS = emulator binconvert
RKT_TARGETS = nandy-asm

INCDIR = include/
# C/C++ files go here
SRCDIR = src/
# Generated object files go here
OBJDIR = obj/

# if you just type "make", this happens
.DEFAULT_GOAL: all
# these rules don't refer to real files, so they shouldn't get timestamp-checked
.PHONY: all clean

# make all = make (all of the final targets given)
all: $(TARGETS) $(RKT_TARGETS)

# includes = everything in INCDIR with a .h file extension
INC = $(wildcard $(INCDIR)*.h)
# C source files = SRCDIR with a .c file extension
CSRC = $(wildcard $(SRCDIR)*.c)
# C++ source files = SRCDIR with a .cpp file extension
CPPSRC = $(wildcard $(SRCDIR)*.cpp)
# Combine the list of filenames from CSRC and CPPSRC but change them to OBJDIR/whatever.o for compiled objects
OBJ = $(patsubst $(SRCDIR)%.c,$(OBJDIR)%.o,$(CSRC)) $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.o,$(CPPSRC))
# Target objects = things that need libraries given to them = convert targets to OBJDIR/whatever.o
TGTOBJ = $(patsubst %,$(OBJDIR)%.o,$(TARGETS))
# Dependency objects = things that should be given to targets = whatever objects aren't targets
DEPOBJ = $(filter-out $(TGTOBJ),$(OBJ))

# Special case for Racket programs
$(RKT_TARGETS): %: $(SRCDIR)%.rkt $(SRCDIR)*.rkt
	raco exe -o $@ $<

# To build a target:
# - Require its corresponding object and all of the dependency objects to be up to date
# - Throw everything to the system's default C++ compiler
$(TARGETS): %: $(OBJDIR)%.o $(DEPOBJ)
	$(CXX) -o $@ $< $(DEPOBJ)

# To build an object:
# - require its .cpp file to be up to date, all includes to be up to date, and the OBJDIR folder to exist
# - Throw it and the includes to default C++ compiler
$(OBJDIR)%.o: $(SRCDIR)%.cpp $(INC) | $(OBJDIR)
	$(CXX) -I$(INCDIR) -c $< -o $@

# If the above rule fails, see if it has a .c file we can use instead of .cpp
$(OBJDIR)%.o: $(SRCDIR)%.c $(INC) | $(OBJDIR)
	$(CC) $(COPTS) -I$(INCDIR) -c $< -o $@

# If the object directory doesn't exist, make it
$(OBJDIR):
	mkdir $@

# If everything else fails, warn the user
% ::
	@echo "File $@ missing or not buildable"

# clean deletes all objects and target executables
clean:
	rm -f $(OBJDIR)* $(TARGETS) $(RKT_TARGETS) *.nb *.logi