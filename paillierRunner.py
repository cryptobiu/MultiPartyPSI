#!/usr/bin/env python

import ConfigParser
import io
import socket
from subprocess import Popen
import thread
import struct
import random
import os

# ssh naor@cybhead1.lnx.biu.ac.il
# ssh cybnode11

MAX_INT = 2**32-1
MIN_INT = 0

class Strategy:
    NAIVE_METHOD_SMALL_N = 0
    NAIVE_METHOD_LARGE_N = 1
    SIMPLE_HASH = 2
    CUCKOO_HASH = 3
    POLYNOMIALS = 4
    BLOOM_FILTER = 5
    BINARY_HASH = 6
    POLYNOMIALS_SIMPLE_HASH = 7
    BINARY_HASH_SIMPLE_HASH = 8
    CUCKOO_HASH_POLYNOMIALS = 9
    CUCKOO_HASH_BLOOM_FILTER = 10
    CUCKOO_HASH_BINARY_HASH = 11

'''
strategies = [Strategy.NAIVE_METHOD_SMALL_N, Strategy.NAIVE_METHOD_LARGE_N, Strategy.SIMPLE_HASH, Strategy.CUCKOO_HASH, Strategy.POLYNOMIALS,
                        Strategy.BLOOM_FILTER, Strategy.BINARY_HASH, Strategy.POLYNOMIALS_SIMPLE_HASH, Strategy.BINARY_HASH_SIMPLE_HASH, Strategy.CUCKOO_HASH_POLYNOMIALS,
                        Strategy.CUCKOO_HASH_BLOOM_FILTER, Strategy.CUCKOO_HASH_BINARY_HASH]
'''

strategies = [Strategy.NAIVE_METHOD_SMALL_N,Strategy.SIMPLE_HASH,Strategy.BLOOM_FILTER,Strategy.POLYNOMIALS,Strategy.POLYNOMIALS_SIMPLE_HASH,]

conf = open("Config", "rb").read()
config = ConfigParser.RawConfigParser(allow_no_value=True)
config.readfp(io.BytesIO(conf))

numOfParties = int(config.get("General", "numOfParties"))

CLOCKS_PER_SEC = 1000000.0

serverIp = config.get("server", "ip")
serverPort = int(config.get("server", "port"))
leaderId = int(config.get("General", "leaderId"))
setSize = int(config.get("General", "setSize"))
seedSizeInBytes=int(config.get("General", "seedSizeInBytes"))

def startPrograms(processes):
    if config.get("General", "debug") == "True":
        for i in xrange(2,numOfParties+1):
            processes.append(Popen(['bin/MultiPartyPSI', str(i),'Config']))
    else:
        for i in xrange(1,numOfParties+1):
            processes.append(Popen(['bin/MultiPartyPSI', str(i),'Config']))

def runMPPSI(strategy):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((serverIp, serverPort))
    s.listen(numOfParties)

    processes = []
    if config.get("General", "remote") == "False":
        thread.start_new_thread(startPrograms, (processes,))
    else:
        for i in xrange(1,numOfParties+1):
            os.system('sshpass -p "305151094" scp ./Config naor@cybnode1%d:Config' % i)
            os.system('sshpass -p "305151094" scp ./bin/MultiPartyPSI naor@cybnode1%d:MultiPartyPSI' % i)
            os.system('sshpass -p "305151094" ssh naor@cybnode1%d "./MultiPartyPSI &"' % i)

    parties = {}
    for _ in xrange(numOfParties):
        conn, addr = s.accept()
        partyId = struct.unpack("<I",conn.recv(4))[0]
        parties[partyId] = conn
        parties[partyId].send(struct.pack("B",strategy))

    for process in processes:
        process.wait()
        print "return code is " + str(process.returncode)

runMPPSI(0)