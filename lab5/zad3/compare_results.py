#!/usr/bin/python3

import sys

input = sys.argv[1]
output = sys.argv[2]

with open(input, "r") as _inp:
    with open(output, "r") as _out:
        output_text = _out.read()
        input_text = _inp.read()
        for line in output_text.splitlines():
            if line == input_text:
                print("Comparison OK")
                exit()
        print("Comparison wrong")
