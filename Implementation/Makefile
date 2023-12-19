PY = python3

FABS = oshpark jlcpcb

PCBDIR = Nandy/
FABDIR = Output/
GRBDIR = Gerbers/
DRCDIR = DRCs/
FAILDIR = FailedDRCs/

PDIRS = $(filter-out $(wildcard $(PCBDIR)*-backups/),$(wildcard $(PCBDIR)*/))
NAMES = $(patsubst $(PCBDIR)%/,%,$(PDIRS))
GRBDIRS = $(patsubst %,$(GRBDIR)%/,$(NAMES))
DIRS = $(PCBDIR) $(FABDIR) $(DRCDIR) $(FAILDIR) $(GRBDIRS)

.PHONY: all clean $(FABS)

all: $(FABS)

$(DIRS): %:
	mkdir -p $@

$(FABDIR)%-oshpark.zip: $(DRCDIR)%.rpt | $(FABDIR) $(GRBDIR)%/
	kikit fab oshpark $(PCBDIR)$*/$*.kicad_pcb $(GRBDIR)$*/
	cp $(GRBDIR)$*/gerbers.zip $@

$(FABDIR)%-jlcpcb.zip: $(DRCDIR)%.rpt | $(FABDIR) $(GRBDIR)%/
	kikit fab jlcpcb $(PCBDIR)$*/$*.kicad_pcb $(GRBDIR)$*/
	cp $(GRBDIR)$*/gerbers.zip $@

.SECONDEXPANSION:
$(DRCDIR)%.rpt: $(PCBDIR)$$*/$$*.kicad_pcb | $(FAILDIR) $(DRCDIR)
	kikit drc run $< > $(FAILDIR)$*.rpt
	mv $(FAILDIR)$*.rpt $@

$(FABS): %: $(patsubst %,$$(FABDIR)%-$$@.zip,$$(NAMES))

clean:
	rm -rf $(DRCDIR) $(FAILDIR) $(GRBDIR) $(FABDIR)
