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
SET_SIZE = 2**2
KEY_SIZES = [80,128]
STRATEGIES = [runner.Strategy.POLYNOMIALS, runner.Strategy.BLOOM_FILTER, runner.Strategy.POLYNOMIALS_SIMPLE_HASH, runner.Strategy.GAUSS_SIMPLE_HASH]
BANDWIDTH_AND_LATENCY = [('1000mbit','0.2ms'),('54mbit','0.2ms'),('25mbit','10ms'),('10mbit','50ms'),('3.6mbit','500ms')]
# and ofcourse kissner
NUM_OF_PARTIES = 5

# sudo tc qdisc add dev lo root tbf rate 1mbit burst 10kb latency 70ms
# sudo tc qdisc add del lo root tbf rate 1mbit burst 10kb latency 70ms

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
    for key_size in KEY_SIZES:
        for strategy in STRATEGIES:
            for i in xrange(10):
                experiment_utils.run_and_add_to_csv('experiment2.csv',NUM_OF_PARTIES,key_size,SET_SIZE,False,strategy,bandwidth,latency)
    set_machines_network(bandwidth, latency,reset=True)
for bandwidth, latency in BANDWIDTH_AND_LATENCY:
    set_machines_network(bandwidth, latency)
    for key_size in KEY_SIZES:
        for i in xrange(10):
            experiment_utils.run_and_add_to_csv('experiment2.csv',NUM_OF_PARTIES,key_size,SET_SIZE,True,None,bandwidth,latency)
    set_machines_network(bandwidth, latency,reset=True)

experiment_utils.avg_experiments('experiment2.csv', 'experiment2_avg.csv', NUM_OF_PARTIES)