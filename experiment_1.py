#!/usr/bin/env python

import runner
import subprocess
import time
import csv
import os
import ConfigParser
import io
import experiment_utils

#SET_SIZES = [2**10,2**12,2**14,2**16,2**18,2**20]
SET_SIZES = [2**2,2**4,2**6,2**8,2**10]
KEY_SIZES = [80,128]
STRATEGIES = [runner.Strategy.POLYNOMIALS, runner.Strategy.BLOOM_FILTER, runner.Strategy.POLYNOMIALS_SIMPLE_HASH, runner.Strategy.GAUSS_SIMPLE_HASH]
# and ofcourse kissner
NUM_OF_PARTIES = 5

experiment_utils.prepare_machines(NUM_OF_PARTIES)

experiment_utils.prepare_results_file('experiment1.csv')

for set_size in SET_SIZES:
    for key_size in KEY_SIZES:
        for strategy in STRATEGIES:
            for i in xrange(10):
                experiment_utils.run_and_add_to_csv('experiment1.csv',NUM_OF_PARTIES,key_size,set_size,False,strategy)
for set_size in SET_SIZES:
    for key_size in KEY_SIZES:
        for i in xrange(10):
            experiment_utils.run_and_add_to_csv('experiment1.csv',NUM_OF_PARTIES,key_size,set_size,True,None)

experiment_utils.avg_experiments('experiment1.csv', 'experiment1_avg.csv', NUM_OF_PARTIES)