import os
os.environ['KICAD7_FOOTPRINT_DIR'] = '/usr/share/kicad/footprints'
import pcbnew
import sys

if len(sys.argv) != 3:
    print(sys.argv[0] + " <source PCB> <DRC dest>")
    exit(1)

board = pcbnew.LoadBoard(sys.argv[1])
pcbnew.WriteDRCReport(board, sys.argv[2], pcbnew.EDA_UNITS_MILS, False)