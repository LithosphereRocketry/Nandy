import sys
import re

if len(sys.argv) != 2:
    print(sys.argv[0] + " <DRC file>")
    exit(1)

with open(sys.argv[1], encoding="utf-8") as f:
    fstr = f.read()
    # Sections 3-6 contain useful data
    drctxt = "\n".join(re.split("\n*\*\* .* \*\*\n*", fstr)[3:6])
    drcsplit = drctxt.split("\n[")
    drcs = [drcsplit[0]] + ["["+str for str in drcsplit[1:]]
    for drc in drcs:
        print("> ", drc)
        if "error" in drc:
            print("Fatal error in DRC")
            exit(1)