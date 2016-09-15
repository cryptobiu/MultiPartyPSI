#!/usr/bin/env python

import csv
from collections import namedtuple
import sys
import math

place = {'BLOOM_FILTER' : 1, 'POLYNOMIALS_SIMPLE_HASH' : 2, 'GAUSS_SIMPLE_HASH' : 3}

dir_name = sys.argv[1]
KEY_SIZES = (80, 128)

with open("{0}/experiment1_avg.csv".format(dir_name), 'rb') as csvfile:
    csvreader = csv.reader(csvfile, delimiter=',', quotechar='|')
    Result = namedtuple('Result',csvreader.next())
    results = []
    
    try:
        while True:
            row = csvreader.next()
            results.append(Result(*row))
    except StopIteration:
        print("done !")

    res = {'BLOOM_FILTER' : {}, 'POLYNOMIALS_SIMPLE_HASH' : {}, 'GAUSS_SIMPLE_HASH' : {}}
    params = []

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
                
            params.append((key_size,set_size))

            set_size = set_size * 4
    
    with open("{0}/experiment1.txt".format(dir_name), 'wb') as f:

        f.write('\\begin{table*}[t]\n')
        f.write('\\hspace*{-2cm}\n')
        f.write('\\begin{tabular}{|l||c|c|c|c|c||c|c|c|c|c|}\n')
        f.write('\\hline\n')
        f.write('\\textbf{Security} & \\multicolumn{5}{c||}{\\textbf{80-bit}} &  \\multicolumn{5}{c|}{\\textbf{128-bit}} \\\\\\hline\n')
        
        f.write('\\textbf{Set Size} & ' + ' & '.join(['\\textbf{{$2^{{{0}}}$}}'.format(int(math.log(set_size,2))) for set_size in sorted(list(set(map(lambda x: x[1], res['BLOOM_FILTER'].keys()))))]*2) + " \\\\\n")
        
        f.write('\\hline\n')
        f.write('\\hline\n')
        
        for strategy in res.keys():
            f.write(" & ".join([strategy.replace('_',' ')]+[res[strategy][param][0] for param in params]) + " \\\\\\hline\n")
        
        f.write('\\end{tabular}\n')
        f.write('\\caption{Runtimes in seconds for PSI protocols over 10Gbit LAN, $\sigma=64$ and 5 parties with different set sizes and security parameters}\n')
        f.write('\\label{tab:results}\n')
        f.write('\\end{table*}\n')

        f.write('\n')

        f.write('\\begin{table*}[t]\n')
        f.write('\\hspace*{-2cm}\n')
        f.write('\\begin{tabular}{|l||c|c|c|c|c||c|c|c|c|c|}\n')
        f.write('\\hline\n')
        f.write('\\textbf{Security} & \\multicolumn{5}{c||}{\\textbf{80-bit}} &  \\multicolumn{5}{c|}{\\textbf{128-bit}} \\\\\\hline\n')
        
        f.write('\\textbf{Set Size} & ' + ' & '.join(['\\textbf{{$2^{{{0}}}$}}'.format(int(math.log(set_size,2))) for set_size in sorted(list(set(map(lambda x: x[1], res['BLOOM_FILTER'].keys()))))]*2) + " \\\\\n")
        
        f.write('\\hline\n')
        f.write('\\hline\n')
        
        for strategy in res.keys():
            f.write(" & ".join([strategy.replace('_',' ')]+[res[strategy][param][1] for param in params]) + " \\\\\\hline\n")
        
        f.write('\\end{tabular}\n')
        f.write('\\caption{Communication complexity (in MB) for PSI protocols over 10Gbit LAN, $\sigma=64$ and 5 parties with different set sizes and security parameters}\n')
        f.write('\\label{tab:results_mb}\n')
        f.write('\\end{table*}\n')
        