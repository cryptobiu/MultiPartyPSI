#!/usr/bin/env python

import csv
from collections import namedtuple
import sys
import math

def readRows(csvfile):
    csvreader = csv.reader(csvfile, delimiter=',', quotechar='|')
    params = csvreader.next()
    num_params = len(params)
    Result = namedtuple('Result',params)
    results = []

    try:
        while True:
            row = csvreader.next()
            row = row + [None]*(num_params-len(row))
            results.append(Result(*row))
    except StopIteration:
        print("done !")
    return results