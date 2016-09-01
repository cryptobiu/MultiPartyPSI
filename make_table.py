#!/usr/bin/env python

import csv
from collections import namedtuple
import sys

place = {'BLOOM_FILTER' : 1, 'POLYNOMIALS_SIMPLE_HASH' : 2, 'GAUSS_SIMPLE_HASH' : 3}

dir_name = sys.argv[1]
KEY_SIZE = int(sys.argv[2])

with open("{0}/experiment1_avg.csv".format(dir_name), 'rb') as csvfile:
    csvreader = csv.reader(csvfile, delimiter=' ', quotechar='|')
    Result = namedtuple('Result',csvreader.next())
    results = []
    
    try:
        while True:
            row = csvreader.next()
            results.append(Result(*row))
    except StopIteration:
        print("done !")
    results = filter(lambda x: x.key_size==str(KEY_SIZE),results)
    results.sort(key=lambda x: int(x.set_size))
    
    with open("{0}/experiment1_{1}.txt".format(dir_name, KEY_SIZE), 'wb') as f:
        f.write('\\begin{table}\n')
        f.write('\\centering\n')
        f.write('\\begin{tabular}{| l | l | l | l | l | l |}\n')
        f.write('\\hline\n')
        f.write('Set Size & Bloom Filter & Polynomials Simple Hash & Gauss Simple Hash \\\\\\hline\n')
        set_size = 2**10
        while len(results) != 0:
            row = filter(lambda x: x.set_size == str(set_size), results)
            results = results[len(row):]
            row.sort(key=lambda x: place[x.strategy])
            vals = map(lambda x: "%.2f" % eval(x.result.split('|')[0])[0], row)
            f.write(" & ".join([str(set_size)] + vals) + " \\\\\\hline\n")
            set_size = set_size * 4
        f.write('\\end{tabular}\n')
        f.write('\\caption{Times (in seconds) with symmetric security parameter %d}\n'%KEY_SIZE)
        f.write('\\end{table}\n')
        f.write('\n')