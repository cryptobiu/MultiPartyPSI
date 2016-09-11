#!/usr/bin/env python

import csv
from collections import namedtuple
import sys
import math

place = {'BLOOM_FILTER' : 1, 'POLYNOMIALS_SIMPLE_HASH' : 2, 'GAUSS_SIMPLE_HASH' : 3}

KEY_SIZE = 80
dir_name = sys.argv[1]
NUM_OF_PARTIES = [3,5,7,10]

with open("{0}/experiment3_avg.csv".format(dir_name), 'rb') as csvfile:
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

    for num_of_parties in NUM_OF_PARTIES:
        row = filter(lambda x: x.num_parties == str(num_of_parties),results)
        
        time_vals = dict(map(lambda x: (x.strategy, "%.2f" % eval(x.result.split('|')[0])[0]), row))

        for strategy in res.keys():
            res[strategy][num_of_parties] = time_vals[strategy]
    
    with open("{0}/experiment3.txt".format(dir_name), 'wb') as f:

        f.write('\\begin{table*}[t]\n')
        f.write('\\hspace*{-2cm}\n')
        f.write('\\begin{tabular}{| l | l | l | l | l | l |}\n')
        f.write('\\hline\n')
        f.write('Num of parties & ' + ' & '.join(["{0}".format(num_of_parties) for num_of_parties in NUM_OF_PARTIES]) + " \\\\\n")
        
        f.write('\\hline\n')
        f.write('\\hline\n')
        
        for strategy in res.keys():
            f.write(" & ".join([strategy.replace('_',' ')]+[res[strategy][num_of_parties] for num_of_parties in NUM_OF_PARTIES]) + " \\\\\\hline\n")
        
        f.write('\\end{tabular}\n')
        f.write('\\caption{Runtimes in seconds for PSI protocols over 10GBit LAN, $\sigma=64$ and set size $2^{16}$ with different number of parties}\n')
        f.write('\\label{tab:results3}\n')
        f.write('\\end{table*}\n')