#!/usr/bin/env python

import runner
import subprocess
import time
import csv
import os
import ConfigParser
import io

#SET_SIZES = [2**10,2**12,2**14,2**16,2**18,2**20]
SET_SIZES = [2**2,2**4,2**6,2**8,2**10]
KEY_SIZES = [80,128]
STRATEGIES = [runner.Strategy.POLYNOMIALS, runner.Strategy.BLOOM_FILTER, runner.Strategy.POLYNOMIALS_SIMPLE_HASH, runner.Strategy.GAUSS_SIMPLE_HASH]
# and ofcourse kissner
NUM_OF_PARTIES = 3

COLUMNS = ""

#ExpResult = namedtuple('ExpResult', ['rev', 'start_time', 'key_size', 'num_parties', 'set_size', 'old_method','strategy'], verbose=True)

os.system('cmake -DCMAKE_BUILD_TYPE=Release CMakeLists.txt; make')
process = subprocess.Popen(['git','rev-parse','HEAD'],stdout=subprocess.PIPE)
REV = process.communicate()[0].rstrip('\n')

if not os.path.isfile('experiment1.csv'):
    with open('experiment1.csv', 'wb') as csvfile:
        csvwriter = csv.writer(csvfile, delimiter=' ',
                               quotechar='|', quoting=csv.QUOTE_MINIMAL)
        csvwriter.writerow(['rev', 'start_time', 'key_size', 'num_parties', 'set_size', 'old_method','strategy', 'result'])
else:
    answer = raw_input("experiment1.csv exists. continuing means it would be appended. do you wish to continue ? (y/n)")
    if answer=='n':
        exit(0)

def run_and_add_to_csv(key_size,set_size,old_method,strategy):
    start_time = time.time()
    result = runner.main(key_size=key_size,num_parties=NUM_OF_PARTIES,set_size=set_size,old_method=old_method,strategy=strategy)
    result_str = '|'.join([str(item[1]) for item in sorted(result.items(),key= lambda x:x[0])])
    row = [REV,str(start_time),str(key_size),str(NUM_OF_PARTIES),str(set_size),str(old_method),str(strategy),result_str]

    with open('experiment1.csv', 'ab') as csvfile:
        csvwriter = csv.writer(csvfile, delimiter=' ',
                                quotechar='|', quoting=csv.QUOTE_MINIMAL)
        csvwriter.writerow(row)

conf = open("BaseConfig", "rb").read()
config = ConfigParser.RawConfigParser(allow_no_value=True)
config.readfp(io.BytesIO(conf))

if config.get("General", "remote") == "True":
    for i in xrange(1,int(config.get("General", "numofparties"))+1):
        ip = config.get(str(i), "ip")
        os.system('ssh -i key.pem {0} "cd MultiPartyPSI; git pull"'.format(ip))
        os.system('scp -i key.pem ./bin/MultiPartyPSI {0}:MultiPartyPSI/MultiPartyPSI'.format(ip))

for set_size in SET_SIZES:
    for key_size in KEY_SIZES:
        for strategy in STRATEGIES:
            for i in xrange(10):
                run_and_add_to_csv(key_size,set_size,False,strategy)
for set_size in SET_SIZES:
    for key_size in KEY_SIZES:
        for i in xrange(10):
            run_and_add_to_csv(key_size,set_size,True,None)

with open('experiment1_avg.csv', 'wb') as csvf:
    csvwriter = csv.writer(csvf, delimiter=' ',
                           quotechar='|', quoting=csv.QUOTE_MINIMAL)
    csvwriter.writerow(['rev', 'start_time', 'key_size', 'num_parties', 'set_size', 'old_method','strategy', 'result'])
    with open('experiment1.csv', 'rb') as csvfile:
        csvreader = csv.reader(csvfile, delimiter=' ', quotechar='|')
        csvreader.next() # remove title
        try:
            while True:
                rows = [csvreader.next() for i in xrange(10)]
                results = [(0,0)]*NUM_OF_PARTIES
                for i in xrange(10):
                    assert rows[i][0] == rows[0][0] #rev
                    assert rows[i][2] == rows[0][2] #key_size
                    assert rows[i][3] == rows[0][3] #num_parties
                    assert rows[i][4] == rows[0][4] #set_size
                    assert rows[i][5] == rows[0][5] #old_method
                    assert rows[i][6] == rows[0][6] #strategy
                    times_and_bytes = [eval(x) for x in rows[i][7].split('|')]
                    results = [(results[i][0]+times_and_bytes[i][0],results[i][1]+times_and_bytes[i][1]) for i in xrange(NUM_OF_PARTIES)]
                results = map(lambda x: str((x[0]/10,x[1]/10)), results)
                rows[0][7]='|'.join(results)
                csvwriter.writerow(rows[0])
        except StopIteration:
            print "done !"
