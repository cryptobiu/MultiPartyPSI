#!/usr/bin/env python

import runner
import subprocess
import time
import csv
import os
import ConfigParser
import io
import experiment_utils

SET_SIZES = [2**2,2**4,2**6,2**8,2**10,2**12,2**14,2**16,2**18]
#KEY_SIZES = [80,128]
KEY_SIZES = [80, 128]
#STRATEGIES = [runner.Strategy.POLYNOMIALS, runner.Strategy.BLOOM_FILTER, runner.Strategy.POLYNOMIALS_SIMPLE_HASH, runner.Strategy.GAUSS_SIMPLE_HASH]
STRATEGIES = [runner.Strategy.SIMPLE_HASH, runner.Strategy.GAUSS_SIMPLE_HASH,
              runner.Strategy.BLOOM_FILTER, runner.Strategy.POLYNOMIALS,
              runner.Strategy.POLYNOMIALS_SIMPLE_HASH, runner.Strategy.TWO_PARTY]
NUM_THREADS = [1,2,3,4,None]

LIMITS = {runner.Strategy.SIMPLE_HASH : 2**14,}

KISSNER_LIMIT=2**8

# and ofcourse kissner
NUM_OF_PARTIES = 5

experiment_utils.prepare_machines(NUM_OF_PARTIES)

experiment_utils.prepare_results_file('experiment1.csv')

FLOW = False

for num_threads in NUM_THREADS:
    for set_size in SET_SIZES:
        for key_size in KEY_SIZES:
            for strategy in STRATEGIES:
                if LIMITS.has_key(strategy) and set_size > LIMITS[strategy]:
                    continue
                print "num_threads: {0}, set_size: {1}, key_size: {2}, strategy: {3}".format(
                    num_threads, set_size, key_size, strategy)
                if not Flow:
                    result = raw_input("skip (s), continue (c)")
                    if result == 's':
                        continue
                    elif result == 'c':
                        FLOW = True
                for i in xrange(10):

                    print "start time: " + time.asctime()
                    while not experiment_utils.run_and_add_to_csv('experiment1.csv',NUM_OF_PARTIES,key_size,set_size,False,strategy,num_threads=num_threads):
                        pass

for set_size in SET_SIZES:
    for key_size in KEY_SIZES:
            if set_size > KISSNER_LIMIT:
                continue
            for i in xrange(10):
                print "set_size: {0}, key_size: {1}, kissner_method".format(
                    set_size, key_size)
                while not experiment_utils.run_and_add_to_csv('experiment1.csv',NUM_OF_PARTIES,key_size,set_size,True,None):
                    pass

experiment_utils.avg_experiments('experiment1.csv', 'experiment1_avg.csv')