#!/usr/bin/env python

import csv
from collections import namedtuple
import sys
import math
import table_utils

place = {'SIMPLE_HASH' : 1, 'GAUSS_SIMPLE_HASH' : 2, 'BLOOM_FILTER' : 3, 'POLYNOMIALS' : 4, 'POLYNOMIALS_SIMPLE_HASH' : 5, 'TWO_PARTY' : 6}

NUM_THREADS = [1,2,4]

KEY_SIZE = 80
SET_SIZE = 2**16
dir_name = sys.argv[1]

with open("{0}/experiment1_avg.csv".format(dir_name), 'rb') as csvfile:
    results = table_utils.readRows(csvfile)

    results = filter(lambda x: x.key_size==str(KEY_SIZE) and x.set_size==str(SET_SIZE),results)

    #res = {'SIMPLE_HASH' : {}, 'GAUSS_SIMPLE_HASH' : {}, 'BLOOM_FILTER' : {}, 'POLYNOMIALS' : {}, 'POLYNOMIALS_SIMPLE_HASH' : {}, 'TWO_PARTY' : {}}
    res = {'SIMPLE_HASH' : {}, 'GAUSS_SIMPLE_HASH' : {}, 'BLOOM_FILTER' : {}, 'POLYNOMIALS_SIMPLE_HASH' : {}, 'TWO_PARTY' : {}}
    params = []

    for num_threads in NUM_THREADS:
        row = filter(lambda x: x.num_threads == str(num_threads),results)

        time_vals = dict(map(lambda x: (x.strategy, "%.2f" % eval(x.result1.split('|')[0])), row))

        for strategy in res.keys():
            res[strategy][num_threads] = time_vals[strategy]
    
    with open("{0}/experiment_threads.txt".format(dir_name), 'wb') as f:

        f.write('\\begin{table*}[t]\n')
        f.write('\\centering\n')
        f.write('\\begin{tabular}{| l | l | l | l | l | l |}\n')
        f.write('\\hline\n')
        f.write('Num threads & ' + ' & '.join(["{0}".format(num_threads) for num_threads in NUM_THREADS]) + " \\\\\n")
        
        f.write('\\hline\n')
        f.write('\\hline\n')
        
        for strategy in res.keys():
            f.write(" & ".join([strategy.replace('_',' ')]+[res[strategy][num_threads] for num_threads in NUM_THREADS]) + " \\\\\\hline\n")
        
        f.write('\\end{tabular}\n')
        f.write('\\caption{Runtimes in seconds for PSI protocols for $\sigma=64$, $\symsec=80$, set size $2^{16}$ and 5 parties with different number of threads}\n')
        f.write('\\label{tab:results_threads}\n')
        f.write('\\end{table*}\n')