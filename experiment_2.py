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
SET_SIZE = 2**16
KEY_SIZE = 80
STRATEGIES = [runner.Strategy.BLOOM_FILTER, runner.Strategy.POLYNOMIALS,
              runner.Strategy.POLYNOMIALS_SIMPLE_HASH, runner.Strategy.TWO_PARTY]
BANDWIDTH_AND_LATENCY = [('3.6mbit','500ms')]
# and ofcourse kissner
NUM_OF_PARTIES = 5

def set_machines_network(rate, latency, interface='eth0', reset=False):
    conf = open("BaseConfig", "rb").read()
    config = ConfigParser.RawConfigParser(allow_no_value=True)
    config.readfp(io.BytesIO(conf))

    cmd = 'add'
    if reset:
        cmd='del'

    cmd_line = ''
    if config.get("General", "remote") == "True":
        for i in xrange(1,int(NUM_OF_PARTIES+1)):
            ip = config.get(str(i), "ip")
            cmd_line = 'ssh -i key.pem {0} "sudo tc qdisc {1} dev {2} root tbf rate {3} burst 10kb latency {4}"'.format(
                ip,cmd,interface,rate,latency)
            os.system(cmd_line)
    else:
        cmd_line = 'sudo tc qdisc {0} dev lo root tbf rate {1} burst 10kb latency {2}'.format(cmd,rate,latency)
        os.system(cmd_line)

def reset_machines_network(interface='eth0'):
    conf = open("BaseConfig", "rb").read()
    config = ConfigParser.RawConfigParser(allow_no_value=True)
    config.readfp(io.BytesIO(conf))

    if config.get("General", "remote") == "True":
        for i in xrange(1,int(NUM_OF_PARTIES+1)):
            ip = config.get(str(i), "ip")
            os.system('ssh -i key.pem {0} "sudo tc qdisc del dev {1} root"'.format(ip, interface))
    else:
        os.system('sudo tc qdisc del dev lo root')

reset_machines_network()

experiment_utils.prepare_machines(NUM_OF_PARTIES)

experiment_utils.prepare_results_file('experiment2.csv')

for bandwidth, latency in BANDWIDTH_AND_LATENCY:
    set_machines_network(bandwidth, latency)
    for strategy in STRATEGIES:
        for i in xrange(10):
            while not experiment_utils.run_and_add_to_csv('experiment2.csv',NUM_OF_PARTIES,KEY_SIZE,SET_SIZE,False,strategy,bandwidth,latency,num_threads=4):
                pass
    set_machines_network(bandwidth, latency,reset=True)

experiment_utils.avg_experiments('experiment2.csv', 'experiment2_avg.csv')