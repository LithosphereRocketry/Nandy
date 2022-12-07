# This should be all you need to change
TARGETS = emulator assembler

# Header files go here
INCDIR = include/
# C/C++ files go here
SRCDIR = src/
# Generated object files go here
OBJDIR = obj/

.DEFAULT_GOAL: all
.PHONY: all clean

all: $(TARGETS)

INC = $(wildcard $(INCDIR)*.h)
CSRC = $(wildcard $(SRCDIR)*.c)
CPPSRC = $(wildcard $(SRCDIR)*.cpp)
OBJ = $(patsubst $(SRCDIR)%.c,$(OBJDIR)%.o,$(CSRC)) $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.o,$(CPPSRC))
TGTOBJ = $(patsubst %,$(OBJDIR)%.o,$(TARGETS))
DEPOBJ = $(filter-out $(TGTOBJ),$(OBJ))

$(TARGETS): %: $(OBJDIR)%.o $(DEPOBJ)
	g++ -o $@ $< $(DEPOBJ)

$(OBJDIR)%.o: $(SRCDIR)%.cpp $(INC) | $(OBJDIR)
	g++ -I$(INCDIR) -c $< -o $@

$(OBJDIR)%.o: $(SRCDIR)%.c $(INC) | $(OBJDIR)
	gcc -I$(INCDIR) -c $< -o $@

$(OBJDIR):
	mkdir $@

% ::
	@echo "File $@ missing or not buildable"


clean:
	rm -f $(OBJDIR)* $(TARGETS)