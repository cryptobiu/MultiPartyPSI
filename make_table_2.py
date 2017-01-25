#!/usr/bin/env python

import csv
from collections import namedtuple
import sys
import math
import table_utils

#place = {'SIMPLE_HASH' : 1, 'GAUSS_SIMPLE_HASH' : 2, 'BLOOM_FILTER' : 3, 'POLYNOMIALS' : 4, 'POLYNOMIALS_SIMPLE_HASH' : 5, 'TWO_PARTY' : 6}
place = {'POLYNOMIALS_SIMPLE_HASH' : 1}

BANDWIDTH_AND_LATENCY = [('1000mbit','0.2ms', 'Gigabit LAN'),('54mbit','0.2ms', '802.11g WiFi'),('25mbit','10ms', 'Intra-country \\\\ WAN'),('10mbit','50ms','Inter-country \\\\ WAN'),('3.6mbit','500ms','HSDPA')]

KEY_SIZE = 80
dir_name = sys.argv[1]

with open("{0}/experiment2_avg.csv".format(dir_name), 'rb') as csvfile:
    results = table_utils.readRows(csvfile)

    results = filter(lambda x: x.key_size==str(KEY_SIZE),results)

    #res = {'SIMPLE_HASH' : {}, 'GAUSS_SIMPLE_HASH' : {}, 'BLOOM_FILTER' : {}, 'POLYNOMIALS' : {}, 'POLYNOMIALS_SIMPLE_HASH' : {}, 'TWO_PARTY' : {}}
    res = {'POLYNOMIALS_SIMPLE_HASH' : {}}
    params = []

    for bandwidth, latency, _ in BANDWIDTH_AND_LATENCY:
        row = filter(lambda x: x.bandwidth == bandwidth and x.latency == latency,results)

        time_vals = dict(map(lambda x: (x.strategy, "%.2f" % eval(x.result1.split('|')[0])), row))

        for strategy in res.keys():
            res[strategy][(bandwidth,latency)] = time_vals[strategy]
    
    with open("{0}/experiment2.txt".format(dir_name), 'wb') as f:

        f.write('\\begin{table*}[t]\n')
        f.write('\\hspace*{-2cm}\n')
        f.write('\\begin{tabular}{| l | l | l | l | l | l |}\n')
        f.write('\\hline\n')
        f.write('\\pbox{10cm}{Network \\\\ Bandwidth, Latency} & ' + ' & '.join(["\\pbox{{10cm}}{{{2} \\\\ {0}, {1}}}".format(bandwidth, latency, description) for bandwidth, latency, description in BANDWIDTH_AND_LATENCY]) + " \\\\\n")
        
        f.write('\\hline\n')
        f.write('\\hline\n')
        
        for strategy in res.keys():
            f.write(" & ".join([strategy.replace('_',' ')]+[res[strategy][(bandwidth,latency)] for bandwidth, latency, _ in BANDWIDTH_AND_LATENCY]) + " \\\\\\hline\n")
        
        f.write('\\end{tabular}\n')
        f.write('\\caption{Runtimes in seconds for PSI protocols for $\sigma=64$, set size $2^{16}$, $\symsec=80$, up to 4 threads and 5 parties in different network scenarios}\n')
        f.write('\\label{tab:results2}\n')
        f.write('\\end{table*}\n')