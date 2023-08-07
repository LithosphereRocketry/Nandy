VLG = iverilog
VVP = vvp

SRCDIR = src/
TESTDIR = tests/
LOGDIR = logs/
OBJDIR = obj/

# Each test gets compiled with all of the files in SRC
SRC = $(wildcard $(SRCDIR)*.v)
# Each test file gets exactly one verilog source
TEST_SRC = $(wildcard $(TESTDIR)*.v)
# Each will be compiled to a wave result
TEST_OUT = $(patsubst $(TESTDIR)%.v,$(LOGDIR)%.txt,$(TEST_SRC))
$(info [${SRC}])

.PHONY: all clean

all: $(TEST_OUT)

$(TEST_OUT): $(LOGDIR)%.txt: $(OBJDIR)%.vvp | $(WAVEDIR)
	$(VVP) $< -l > $@

$(OBJDIR)%.vvp: $(TESTDIR)%.v $(SRC) | $(OBJDIR)
	$(VLG) -s $* -o $@ $^

# If the directories don't exist, make them
$(WAVEDIR):
	mkdir $@
$(OBJDIR):
	mkdir $@

# If everything else fails, warn the user
% ::
	@echo "File $@ missing or not buildable"

clean:
	rm -f waveforms/*.vd obj/*.vvp