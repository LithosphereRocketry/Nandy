import pcbnew
import sys

if len(sys.argv) != 3:
    print(sys.argv[0] + " <source PCB> <Gerbers dir>")
    exit(1)

board = pcbnew.LoadBoard(sys.argv[1])
print("\n".join(dir(board)))