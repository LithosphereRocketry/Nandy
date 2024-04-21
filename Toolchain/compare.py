import os
import subprocess

directory = 'tests'
for fname in os.listdir(directory):
    progname = ".".join(fname.split(".")[:-1])
    f = os.path.join(directory, fname)
    if os.path.isfile(f):
        if subprocess.run(["./nandy-asm", f, progname+".nb"]).returncode != 0:
            print("C assembly failed on ", f)
            exit(-1)
        elif subprocess.run(["./nandy-asm-racket", f, progname+"_rkt.nb"]).returncode != 0:
            print("C assembly failed on ", f)
            exit(-1)
        cbin = open(progname+".nb", "rb").read()
        rbin = open(progname+"_rkt.nb", "rb").read()
        if cbin != rbin:
            print("Assembly did not match on ", f)
            exit(-1)
exit(0)