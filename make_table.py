#!/usr/bin/env python

import csv
from collections import namedtuple
import sys
import math
import table_utils

#place = {'SIMPLE_HASH' : 1, 'GAUSS_SIMPLE_HASH' : 2, 'BLOOM_FILTER' : 3, 'POLYNOMIALS' : 4, 'POLYNOMIALS_SIMPLE_HASH' : 5, 'TWO_PARTY' : 6}
place = {'POLYNOMIALS_SIMPLE_HASH' : 1, 'BLOOM_FILTER' : 2}

dir_name = sys.argv[1]
KEY_SIZES = (80, 128)
NUM_OF_THREADS = None

with open("{0}/experiment1_avg.csv".format(dir_name), 'rb') as csvfile:
    results = table_utils.readRows(csvfile)

    results = filter(lambda x: x.num_threads==str(NUM_OF_THREADS),results)

    #res = {'SIMPLE_HASH' : {}, 'GAUSS_SIMPLE_HASH' : {}, 'BLOOM_FILTER' : {}, 'POLYNOMIALS' : {}, 'POLYNOMIALS_SIMPLE_HASH' : {}, 'TWO_PARTY' : {}}
    res = {'POLYNOMIALS_SIMPLE_HASH' : {}, 'BLOOM_FILTER' : {}}
    params = []

    for key_size in KEY_SIZES:
        data = filter(lambda x: x.key_size==str(key_size),results)
        data.sort(key=lambda x: int(x.set_size))
        set_size = 2**10
        while set_size < 2**20:
            row = filter(lambda x: x.set_size == str(set_size), data)
            row.sort(key=lambda x: place[x.strategy])

            time_vals = dict(map(lambda x: (x.strategy, "%.2f" % eval(x.result1.split('|')[0])), row))

            byte_vals = dict(map(lambda x: (x.strategy, "%.2f"% (sum([eval(y.split('|')[1]) for y in
                                                                     [getattr(x, 'result{0}'.format(id)) for id in xrange(1, 10+1)] if y is not None])/1000000.0)),row))

            for strategy in res.keys():
                if time_vals.has_key(strategy):
                    res[strategy][(key_size,set_size)] = (time_vals[strategy], byte_vals[strategy])
                else:
                    res[strategy][(key_size,set_size)] = (' ', ' ')
                
            params.append((key_size,set_size))

            set_size = set_size * 4
    
    with open("{0}/experiment1.txt".format(dir_name), 'wb') as f:

        f.write('\\begin{table*}[t]\n')
        f.write('\\hspace*{-2cm}\n')
        f.write('\\begin{tabular}{|l||c|c|c|c|c||c|c|c|c|c|}\n')
        f.write('\\hline\n')
        f.write('\\textbf{Security} & \\multicolumn{5}{c||}{\\textbf{80-bit}} &  \\multicolumn{5}{c|}{\\textbf{128-bit}} \\\\\\hline\n')
        
        f.write('\\textbf{Set Size} & ' + ' & '.join(['\\textbf{{$2^{{{0}}}$}}'.format(int(math.log(set_size,2))) for set_size in sorted(list(set(map(lambda x: x[1], res['POLYNOMIALS_SIMPLE_HASH'].keys()))))]*2) + " \\\\\n")
        
        f.write('\\hline\n')
        f.write('\\hline\n')
        
        for strategy in res.keys():
            f.write(" & ".join([strategy.replace('_',' ')]+[res[strategy][param][0] for param in params]) + " \\\\\\hline\n")
        
        f.write('\\end{tabular}\n')
        f.write('\\caption{Runtimes in seconds for PSI protocols over 10Gbit LAN, $\sigma=64$, infinite number of threads and 5 parties with different set sizes and security parameters}\n')
        f.write('\\label{tab:results}\n')
        f.write('\\end{table*}\n')

        f.write('\n')

        f.write('\\begin{table*}[t]\n')
        f.write('\\hspace*{-2cm}\n')
        f.write('\\begin{tabular}{|l||c|c|c|c|c||c|c|c|c|c|}\n')
        f.write('\\hline\n')
        f.write('\\textbf{Security} & \\multicolumn{5}{c||}{\\textbf{80-bit}} &  \\multicolumn{5}{c|}{\\textbf{128-bit}} \\\\\\hline\n')
        
        f.write('\\textbf{Set Size} & ' + ' & '.join(['\\textbf{{$2^{{{0}}}$}}'.format(int(math.log(set_size,2))) for set_size in sorted(list(set(map(lambda x: x[1], res['POLYNOMIALS_SIMPLE_HASH'].keys()))))]*2) + " \\\\\n")
        
        f.write('\\hline\n')
        f.write('\\hline\n')
        
        for strategy in res.keys():
            f.write(" & ".join([strategy.replace('_',' ')]+[res[strategy][param][1] for param in params]) + " \\\\\\hline\n")
        
        f.write('\\end{tabular}\n')
        f.write('\\caption{Communication complexity (in MB) for PSI protocols over 10Gbit LAN, $\sigma=64$, infinite number of threads and 5 parties with different set sizes and security parameters}\n')
        f.write('\\label{tab:results_mb}\n')
        f.write('\\end{table*}\n')
        