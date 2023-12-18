PY = python3

PCBDIR = Nandy/
FABDIR = Output/
GRBDIR = Gerbers/
DRCDIR = DRCs/

PDIRS = $(filter-out $(wildcard $(PCBDIR)*-backups/),$(wildcard $(PCBDIR)*/))
ZIPS = $(patsubst $(PCBDIR)%/,$(FABDIR)%.zip,$(PDIRS))

DIRS = PCBDIR FABDIR GRBDIR DRCDIR



$(DIRS): %:
	mkdir $@
