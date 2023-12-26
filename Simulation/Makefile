VLG = iverilog
VVP = vvp
PY = python3

SRCDIR = src/
TESTDIR = tests/
LOGDIR = logs/
OBJDIR = obj/
INCDIR = include/
WAVEDIR = waveforms/

TMPFILE = $(LOGDIR)TEMP.txt
FUZZFILE = $(TESTDIR)FUZZ.txt
MAKEFUZZ = romfuzz.py

# Each test gets compiled with all of the files in SRC
SRC = $(wildcard $(SRCDIR)*.v)
INC = $(wildcard $(INCDIR)*.vh)
# Each test file gets exactly one verilog source
TEST_SRC = $(wildcard $(TESTDIR)*.v)
# Each will be compiled to a wave result
TEST_OUT = $(patsubst $(TESTDIR)%.v,$(LOGDIR)%.txt,$(TEST_SRC))

.PHONY: all clean

all: $(TEST_OUT)

$(FUZZFILE): $(MAKEFUZZ)
	$(PY) $(MAKEFUZZ) > $@

$(TEST_OUT): $(LOGDIR)%.txt: $(OBJDIR)%.vvp $(FUZZFILE) | $(LOGDIR)
	$(VVP) $< > $@
	@if ((grep -Fxq "ERROR:" $@)); then cat $@; exit 1; fi

$(OBJDIR)%.vvp: $(TESTDIR)%.v $(SRC) $(INC) | $(OBJDIR) $(WAVEDIR)
	$(VLG) -DWAVEPATH=\"$(WAVEDIR)$*.vcd\" -I$(INCDIR) -s $* -o $@ $< $(SRC)

# If the directories don't exist, make them
$(WAVEDIR):
	mkdir $@
$(LOGDIR):
	mkdir $@
$(OBJDIR):
	mkdir $@

# If everything else fails, warn the user
% ::
	@echo "File $@ missing or not buildable"
	@false

clean:
	rm -f $(WAVEDIR)*.vcd $(OBJDIR)*.vvp $(LOGDIR)*.txt