VLG = iverilog
VVP = vvp

SRCDIR = src/
TESTDIR = tests/
LOGDIR = logs/
OBJDIR = obj/
INCDIR = include/

# Each test gets compiled with all of the files in SRC
SRC = $(wildcard $(SRCDIR)*.v)
INC = $(wildcard $(INCDIR)*.vh)
# Each test file gets exactly one verilog source
TEST_SRC = $(wildcard $(TESTDIR)*.v)
# Each will be compiled to a wave result
TEST_OUT = $(patsubst $(TESTDIR)%.v,$(LOGDIR)%.txt,$(TEST_SRC))
$(info [${SRC}])

.PHONY: all clean

all: $(TEST_OUT)

$(TEST_OUT): $(LOGDIR)%.txt: $(OBJDIR)%.vvp | $(LOGDIR)
	$(VVP) $< > $@

$(OBJDIR)%.vvp: $(TESTDIR)%.v $(SRC) $(INC) | $(OBJDIR)
	$(VLG) -I$(INCDIR) -s $* -o $@ $< $(SRC)

# If the directories don't exist, make them
$(WAVEDIR):
	mkdir $@
$(LOGDIR):
	mkdir $@

# If everything else fails, warn the user
% ::
	@echo "File $@ missing or not buildable"

clean:
	rm -f waveforms/*.vd obj/*.vvp