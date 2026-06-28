VLG = iverilog
VVP = vvp
PY = python3

SRCDIR = src
FPGADIR = fpga
SIMDIR = sim
TESTDIR = tests/
LOGDIR = logs/
ERRDIR = error-logs/
OBJDIR = obj
WAVEDIR = waveforms/

TMPFILE = $(LOGDIR)TEMP.txt
FUZZFILE = $(TESTDIR)FUZZ.txt
MAKEFUZZ = romfuzz.py

# Each test gets compiled with all of the files in SRC
SRC = $(wildcard $(SRCDIR)/*.v)
FPGA_SRC = $(wildcard $(FPGADIR)/*.v)
SIM_SRC =  $(wildcard $(SIMDIR)/*.v)
# Each test file gets exactly one verilog source
TEST_SRC = $(wildcard $(TESTDIR)*.v)
# Each will be compiled to a wave result
TEST_OUT = $(patsubst $(TESTDIR)%.v,$(LOGDIR)vl_%.txt,$(TEST_SRC))

.PHONY: all clean nextpnrgui dfu lint

all: $(TEST_OUT)

$(FUZZFILE): $(MAKEFUZZ)
	$(PY) $(MAKEFUZZ) > $@

$(TEST_OUT): $(LOGDIR)vl_%.txt: $(OBJDIR)/%.vvp $(FUZZFILE) | $(LOGDIR) $(ERRDIR)
	$(VVP) $< > $(ERRDIR)vl_$*.txt
	@! grep -Fq "ERROR:" $(ERRDIR)vl_$*.txt
	@mv $(ERRDIR)vl_$*.txt $@

$(OBJDIR)/%.vvp: $(TESTDIR)%.v $(SRC) $(SIM_SRC) | $(OBJDIR) $(WAVEDIR)
	$(VLG) -DWAVEPATH=\"$(WAVEDIR)vl_$*.vcd\" -s $* -o $@ $^

# If the directories don't exist, make them
$(WAVEDIR):
	mkdir $@
$(LOGDIR):
	mkdir $@
$(ERRDIR):
	mkdir $@
$(OBJDIR):
	mkdir $@

# FPGA nonsense

TOPLEVEL = toplevel
PCF = orangecrab_r0.2.1.pcf
NEXTPNR_DENSITY = 85k

rom.nb:
	@echo "Copy an assembled NANDy program into rom.nb and try again"
	@exit -1

$(OBJDIR)/rom.hex: rom.nb | $(OBJDIR)
	xxd -p -c 1 $< > $@

$(OBJDIR)/%.ys: $(FPGA_SRC) $(SRC) $(OBJDIR)/rom.hex | $(OBJDIR)
	$(file >$@)
	$(foreach V,$(SRC) $(FPGA_SRC),$(file >>$@,read_verilog -DROMPATH="$(OBJDIR)/rom.hex" -DSYNTH=1 $V))
	$(file >>$@,synth_ecp5 -top $*)
	$(file >>$@,write_json "$(basename $@).json")

$(OBJDIR)/%.json: $(OBJDIR)/%.ys | $(OBJDIR)
	yosys -s "$<" > yosys-log.txt

$(OBJDIR)/%_out.config $(OBJDIR)/%.pnr.json: $(OBJDIR)/%.json $(PCF) | $(OBJDIR)
	nextpnr-ecp5 --json $< --textcfg $(OBJDIR)/$*_out.config --$(NEXTPNR_DENSITY) --package CSFBGA285 --lpf $(PCF) --write $(OBJDIR)/$*.pnr.json 2> nextpnr-log.txt

nextpnrgui: $(OBJDIR)/$(TOPLEVEL).pnr.json
	nextpnr-ecp5 --json $< --$(NEXTPNR_DENSITY) --package CSFBGA285 --lpf $(PCF) --gui &

$(OBJDIR)/%.bit: $(OBJDIR)/%_out.config | $(OBJDIR)
	ecppack --compress --freq 38.8 --input $< --bit $@

$(OBJDIR)/%.dfu: $(OBJDIR)/%.bit | $(OBJDIR)
	cp $< $@
	dfu-suffix -v 1209 -p 5af0 -a $@

dfu: $(OBJDIR)/$(TOPLEVEL).dfu | $(OBJDIR)
	dfu-util --alt 0 -D $<

lint: $(FPGA_SRC) $(SRC) $(OBJDIR)/rom.hex
	verilator --lint-only --top-module $(TOPLEVEL) $(SRC) $(FPGA_SRC) -DROMPATH=\"$(OBJDIR)/rom.hex\"

clean:
	rm -rf $(WAVEDIR) $(OBJDIR) $(LOGDIR) $(ERRDIR)
