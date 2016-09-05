#!/usr/bin/env python

import csv
from collections import namedtuple
import sys
import math

place = {'BLOOM_FILTER' : 1, 'POLYNOMIALS_SIMPLE_HASH' : 2, 'GAUSS_SIMPLE_HASH' : 3}
BANDWIDTH_AND_LATENCY = [('1000mbit','0.2ms'),('54mbit','0.2ms'),('25mbit','10ms'),('10mbit','50ms'),('3.6mbit','500ms')]

KEY_SIZE = 80
dir_name = sys.argv[1]

with open("{0}/experiment2_avg.csv".format(dir_name), 'rb') as csvfile:
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
    
    with open("{0}/experiment2_{1}.txt".format(dir_name, KEY_SIZE), 'wb') as f:
        data = results
        f.write('\\begin{table}\n')
        f.write('\\centering\n')
        f.write('\\begin{tabular}{| l | l | l | l | l | l |}\n')
        f.write('\\hline\n')
        f.write('Bandwidth & Latency & Bloom Filter & Polynomials Simple Hash & Gauss Simple Hash \\\\\\hline\n')
        for bandwidth, latency in BANDWIDTH_AND_LATENCY:
            row = filter(lambda x: x.bandwidth == bandwidth and x.latency == latency, data)
            row.sort(key=lambda x: place[x.strategy])
            vals = map(lambda x: "%.2f" % eval(x.result.split('|')[0])[0], row)
            f.write(" & ".join([bandwidth, latency] + vals) + " \\\\\\hline\n")
        f.write('\\end{tabular}\n')
        f.write('\\caption{Times (in seconds) with symmetric security parameter %d}\n'%KEY_SIZE)
        f.write('\\end{table}\n')