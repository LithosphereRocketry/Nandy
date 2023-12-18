PY = python3

PCBDIR = Nandy/
FABDIR = Output/
GRBDIR = Gerbers/
DRCDIR = DRCs/

PDIRS = $(filter-out $(wildcard $(PCBDIR)*-backups/),$(wildcard $(PCBDIR)*/))
NAMES = $(patsubst $(PCBDIR)%/,%,$(PDIRS))
DRCS = $(patsubst %,$(DRCDIR)%.rpt,$(NAMES))
VERIFIES = $(patsubst %,__VERIFY_%,$(NAMES))
ZIPS = $(patsubst %,$(FABDIR)%.zip,$(NAMES))
DIRS = $(PCBDIR) $(FABDIR) $(GRBDIR) $(DRCDIR)

.PHONY: clean $(VERIFIES)

.DEFAULT: all

all: __VERIFY_register

$(ZIPS): $(FABDIR)%.zip: __VERIFY_%

.SECONDEXPANSION:
$(DRCS): $(DRCDIR)%.rpt: $(PCBDIR)$$*/$$*.kicad_pcb | $(DRCDIR)
	$(PY) run-drc.py $< $@

$(VERIFIES): __VERIFY_%: $(DRCDIR)%.rpt
	$(PY) verify-drc.py $<

$(DIRS): %:
	mkdir $@
