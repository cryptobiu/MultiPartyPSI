#!/usr/bin/env python

import runner
import subprocess
import time
import csv
import os
import ConfigParser
import io
import experiment_utils

SET_SIZE = 2**16
KEY_SIZE = 80
'''
STRATEGIES = [runner.Strategy.SIMPLE_HASH, runner.Strategy.GAUSS_SIMPLE_HASH,
              runner.Strategy.BLOOM_FILTER, runner.Strategy.POLYNOMIALS,
              runner.Strategy.POLYNOMIALS_SIMPLE_HASH, runner.Strategy.TWO_PARTY]
'''

STRATEGIES = [runner.Strategy.POLYNOMIALS_SIMPLE_HASH, runner.Strategy.TWO_PARTY]

#NUM_OF_PARTIES = [2,3,5,7,9,10]
NUM_OF_PARTIES = [6]

experiment_utils.prepare_machines(NUM_OF_PARTIES[-1])

experiment_utils.prepare_results_file('experiment3.csv')

for num_of_parties in NUM_OF_PARTIES:
    for strategy in STRATEGIES:
        for i in xrange(10):
            print "start time: " + time.asctime()
            while not experiment_utils.run_and_add_to_csv('experiment3.csv',num_of_parties,KEY_SIZE,SET_SIZE,False,strategy,num_threads=4):
                pass


experiment_utils.avg_experiments('experiment3.csv', 'experiment3_avg.csv')