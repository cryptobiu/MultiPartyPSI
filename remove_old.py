#!/usr/bin/env python

import csv
import sys
import time
import subprocess
import os
import ConfigParser
import io

MAX_NUM_PARTIES = 10

result_file_path = sys.argv[1]
new_result_file_path = result_file_path + "_new"
print new_result_file_path
with open(new_result_file_path, 'wb') as csvf:
    csvwriter = csv.writer(csvf, delimiter=',',
                           quotechar='|', quoting=csv.QUOTE_MINIMAL)
    csvwriter.writerow(['rev', 'bandwidth', 'latency', 'start_time', 'key_size', 'num_parties', 'num_threads', 'set_size', 'old_method','strategy'] +
                       ['result{0}'.format(i) for i in xrange(1,MAX_NUM_PARTIES+1)])
    with open(result_file_path, 'rb') as csvfile:
        csvreader = csv.reader(csvfile, delimiter=',', quotechar='|')
        csvreader.next() # remove title
        try:
            while True:
                row = csvreader.next()

                if row[7] != '1048576' and row[9] == 'POLYNOMIALS_SIMPLE_HASH':
                    continue

                csvwriter.writerow(row)
        except StopIteration:
            print "done !"