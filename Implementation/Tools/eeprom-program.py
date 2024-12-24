#!/usr/bin/python3
import sys
import tqdm
import serial

WORDLEN = 64

with open(sys.argv[1], 'rb') as infile:
    bytearr = infile.read()
    words = [bytearr[i:i+WORDLEN] for i in range(0, len(bytearr), WORDLEN)]
    with serial.Serial(sys.argv[2], 4000000) as port:
        print("Writing", len(bytearr), "bytes...")
        for w in tqdm.tqdm(words, "pages"):
            port.write(w)
            port.read(1) # wait for acknowledge
        port.timeout = 10
        print("Reading back...")
        resp = port.read(len(bytearr))
        if resp != bytearr:
            print("Error in transmission, got ", resp.decode("ASCII"))
        else:
            print("Done!")