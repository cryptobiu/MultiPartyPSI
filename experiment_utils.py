#!/usr/bin/env python

import runner
import csv
import time
import subprocess
import os
import ConfigParser
import io

process = subprocess.Popen(['git','rev-parse','HEAD'],stdout=subprocess.PIPE)
REV = process.communicate()[0].rstrip('\n')

NUM_OF_RUNS = 10
MAX_NUM_PARTIES = 10

def prepare_machines(num_of_parties):
    conf = open("BaseConfig", "rb").read()
    config = ConfigParser.RawConfigParser(allow_no_value=True)
    config.readfp(io.BytesIO(conf))

    if config.get("General", "remote") == "True":
        for i in xrange(1,int(num_of_parties+1)):
            ip = config.get(str(i), "ip")
            os.system('ssh -i key.pem {0} "cd MultiPartyPSI; git pull"'.format(ip))
            os.system('ssh -i key.pem {0} "rm MultiPartyPSI/MultiPartyPSI"'.format(ip))

            if i == 1:
                os.system('ssh -i key.pem {0} "cd MultiPartyPSI; cmake -DCMAKE_BUILD_TYPE=Release CMakeLists.txt; make"'.format(ip))
                os.system('mkdir bin')
                os.system('scp -i key.pem {0}:MultiPartyPSI/bin/MultiPartyPSI ./bin/MultiPartyPSI'.format(ip))

            os.system('scp -i key.pem ./bin/MultiPartyPSI {0}:MultiPartyPSI/MultiPartyPSI'.format(ip))
    else:
        os.system('cmake -DCMAKE_BUILD_TYPE=Release CMakeLists.txt; make')

def run_and_add_to_csv(results_file_path,num_of_parties,key_size,set_size,old_method,strategy,bandwidth=None,latency=None,num_threads=None):
    start_time = time.time()
    print "Run with bandwidth {0} and latency {1}".format(bandwidth,latency)
    result = runner.main(key_size=key_size,num_parties=num_of_parties,set_size=set_size,old_method=old_method,strategy=strategy,num_threads=num_threads)
    if len(result) != num_of_parties:
        return False
    final_results = ['|'.join(map(str, list(item[1]))) for item in sorted(result.items(),key= lambda x:x[0])]
    row = [REV,str(bandwidth),str(latency),str(start_time),str(key_size),str(num_of_parties), str(num_threads),
           str(set_size),str(old_method),runner.getStrategyName(strategy)] + final_results

    with open(results_file_path, 'ab') as csvfile:
        csvwriter = csv.writer(csvfile, delimiter=',',
                               quotechar='|', quoting=csv.QUOTE_MINIMAL)
        csvwriter.writerow(row)
    return True

def prepare_results_file(config_file_path):
    if not os.path.isfile(config_file_path):
        with open(config_file_path, 'wb') as csvfile:
            csvwriter = csv.writer(csvfile, delimiter=',',
                                   quotechar='|', quoting=csv.QUOTE_MINIMAL)
            csvwriter.writerow(['rev', 'bandwidth', 'latency', 'start_time', 'key_size', 'num_parties', 'num_threads', 'set_size', 'old_method','strategy'] +
                                ['result{0}'.format(i) for i in xrange(1,MAX_NUM_PARTIES+1)])
    else:
        answer = raw_input("{0} exists. continuing means it would be appended. do you wish to continue ? (y/n)".format(config_file_path))
        if answer=='n':
            exit(0)

def avg_experiments(result_file_path, avg_result_file_path):
    with open(avg_result_file_path, 'wb') as csvf:
        csvwriter = csv.writer(csvf, delimiter=',',
                               quotechar='|', quoting=csv.QUOTE_MINIMAL)
        csvwriter.writerow(['rev', 'bandwidth', 'latency', 'start_time', 'key_size', 'num_parties', 'num_threads', 'set_size', 'old_method','strategy'] +
                           ['result{0}'.format(i) for i in xrange(1,MAX_NUM_PARTIES+1)])
        with open(result_file_path, 'rb') as csvfile:
            csvreader = csv.reader(csvfile, delimiter=',', quotechar='|')
            csvreader.next() # remove title
            try:
                while True:
                    rows = [csvreader.next() for i in xrange(NUM_OF_RUNS)]
                    results = None
                    for i in xrange(NUM_OF_RUNS):
                        print rows[i][0]
                        assert rows[i][0] == rows[0][0] #rev
                        assert rows[i][1] == rows[0][1] #bandwidth
                        assert rows[i][2] == rows[0][2] #latency

                        assert rows[i][4] == rows[0][4] #key_size
                        assert rows[i][5] == rows[0][5] #num_parties
                        assert rows[i][6] == rows[0][6] #num_threads
                        assert rows[i][7] == rows[0][7] #set_size
                        assert rows[i][8] == rows[0][8] #old_method
                        assert rows[i][9] == rows[0][9] #strategy

                        times_and_bytes = []
                        for j in xrange(10, len(rows[i])):
                            times_and_bytes.append(tuple(map(eval, rows[i][j].split('|'))))

                        num_of_parties = len(times_and_bytes)
                        if results is None:
                            results = [(0,0,0,0)]*num_of_parties
                        results = [(results[i][0]+times_and_bytes[i][0],results[i][1]+times_and_bytes[i][1],
                                    results[i][2]+times_and_bytes[i][2], results[i][3]+times_and_bytes[i][3]) for i in xrange(num_of_parties)]
                    results = map(lambda x: '|'.join([str(x[0]/NUM_OF_RUNS),str(x[1]/NUM_OF_RUNS),str(x[2]/NUM_OF_RUNS),str(x[3]/NUM_OF_RUNS)]), results)
                    csvwriter.writerow(rows[0][:10] + results)
            except StopIteration:
                print "done !"
