#!/usr/bin/env python

import csv
from collections import namedtuple
import sys
import math

place = {'BLOOM_FILTER' : 1, 'POLYNOMIALS_SIMPLE_HASH' : 2, 'GAUSS_SIMPLE_HASH' : 3}

dir_name = sys.argv[1]
KEY_SIZES = (80, 128)

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

    res = {'BLOOM_FILTER' : {}, 'POLYNOMIALS_SIMPLE_HASH' : {}, 'GAUSS_SIMPLE_HASH' : {}}

    for key_size in KEY_SIZES:
        data = filter(lambda x: x.key_size==str(key_size),results)
        data.sort(key=lambda x: int(x.set_size))
        set_size = 2**10
        while len(data) != 0:
            row = filter(lambda x: x.set_size == str(set_size), data)
            data = data[len(row):]
            row.sort(key=lambda x: place[x.strategy])

            time_vals = dict(map(lambda x: (x.strategy, "%.2f" % eval(x.result.split('|')[0])[0]), row))
            byte_vals = dict(map(lambda x: (x.strategy, "%.2f"% (sum(map(lambda y: eval(y)[1],x.result.split('|')))/1000000.0)), row))

            for strategy in res.keys():
                res[strategy][(key_size,set_size)] = (time_vals[strategy], byte_vals[strategy])

            set_size = set_size * 4
    import pdb; pdb.set_trace()

    with open("{0}/experiment1_{1}.txt".format(dir_name, KEY_SIZE), 'wb') as f:

        f.write('\\begin{table}\n')
        f.write('\\centering\n')
        f.write('\\begin{tabular}{| l | l | l | l | l | l |}\n')
        f.write('\\hline\n')
        f.write('Set Size & Bloom Filter & Polynomials Simple Hash & Gauss Simple Hash \\\\\\hline\n')
        #f.write(" & ".join(['$2^{{{0}}}$'.format(int(math.log(set_size,2)))] + vals) + " \\\\\\hline\n")
        f.write('\\end{tabular}\n')
        f.write('\\caption{Times (in seconds) with symmetric security parameter %d}\n'%KEY_SIZE)
        f.write('\\end{table}\n')

        f.write('\n')

        data = results
        f.write('\\begin{table}\n')
        f.write('\\centering\n')
        f.write('\\begin{tabular}{| l | l | l | l | l | l |}\n')
        f.write('\\hline\n')
        f.write('Set Size & Bloom Filter & Polynomials Simple Hash & Gauss Simple Hash \\\\\\hline\n')
        f.write('\\end{tabular}\n')
        f.write('\\caption{Communication complexity (in MB) with symmetric security parameter %d}\n'%KEY_SIZE)
        f.write('\\end{table}\n')