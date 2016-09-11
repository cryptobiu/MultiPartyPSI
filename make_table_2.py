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
    
    res = {'BLOOM_FILTER' : {}, 'POLYNOMIALS_SIMPLE_HASH' : {}, 'GAUSS_SIMPLE_HASH' : {}}
    params = []

    for bandwidth, latency in BANDWIDTH_AND_LATENCY:
        row = filter(lambda x: x.bandwidth == bandwidth and x.latency == latency,results)
        
        time_vals = dict(map(lambda x: (x.strategy, "%.2f" % eval(x.result.split('|')[0])[0]), row))

        for strategy in res.keys():
            res[strategy][(bandwidth,latency)] = time_vals[strategy]
    
    with open("{0}/experiment2.txt".format(dir_name), 'wb') as f:

        f.write('\\begin{table*}[t]\n')
        f.write('\\centering\n')
        f.write('\\begin{tabular}{| l | l | l | l | l | l |}\n')
        f.write('\\hline\n')
        f.write(' & ' + ' & '.join(["{0}, {1}".format(bandwidth, latency) for bandwidth, latency in BANDWIDTH_AND_LATENCY]) + " \\\\\n")
        
        f.write('\\hline\n')
        f.write('\\hline\n')
        
        for strategy in res.keys():
            f.write(" & ".join([strategy.replace('_',' ')]+[res[strategy][(bandwidth,latency)] for bandwidth, latency in BANDWIDTH_AND_LATENCY]) + " \\\\\\hline\n")
        
        f.write('\\end{tabular}\n')
        f.write('\\caption{Times (in seconds) for 5 parties}\n')
        f.write('\\label{tab:results}\n')
        f.write('\\end{table*}\n')
    '''
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
    '''