#!/usr/bin/env python

import ConfigParser
import io
import socket
from subprocess import Popen
import thread
import struct
import random
import os

#docker images
#docker run -it scapicryptobiu/multipartypsi
# docker rmi -f scapicryptobiu/multipartypsi
# docker build --no-cache -t scapicryptobiu/multipartypsi:latest .

# dpkg-divert --local --rename --add /sbin/initctl
# ln -s /bin/true /sbin/initctl

# sudo service docker stop
# sudo pkill -f docker
# sudo service docker start

# ssh naor@cybhead1.lnx.biu.ac.il
# ssh cybnode11

MAX_INT = 2**32-1
MIN_INT = 0
PROGRAM_TYPE = 0
LOOPBACK_ADDRESS = "127.0.0.1"
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
    GAUSS_SIMPLE_HASH = 12

'''
strategies = [Strategy.NAIVE_METHOD_SMALL_N, Strategy.NAIVE_METHOD_LARGE_N, Strategy.SIMPLE_HASH, Strategy.POLYNOMIALS,
                        Strategy.BLOOM_FILTER, Strategy.POLYNOMIALS_SIMPLE_HASH, Strategy.BINARY_HASH, Strategy.BINARY_HASH_SIMPLE_HASH, Strategy.CUCKOO_HASH, Strategy.CUCKOO_HASH_POLYNOMIALS,
                        Strategy.CUCKOO_HASH_BLOOM_FILTER, Strategy.CUCKOO_HASH_BINARY_HASH, Strategy.GAUSS_SIMPLE_HASH]
'''
'''Strategy.NAIVE_METHOD_SMALL_N,Strategy.NAIVE_METHOD_LARGE_N,Strategy.SIMPLE_HASH,'''
strategies = [Strategy.GAUSS_SIMPLE_HASH, Strategy.BLOOM_FILTER,Strategy.POLYNOMIALS,Strategy.POLYNOMIALS_SIMPLE_HASH]

conf = open("Config", "rb").read()
config = ConfigParser.RawConfigParser(allow_no_value=True)
config.readfp(io.BytesIO(conf))

numOfParties = int(config.get("General", "numOfParties"))

CLOCKS_PER_SEC = 1000000.0

isLocalHost = (config.get("General", "remote") == "False")

serverIp = config.get("server", "ip")
if isLocalHost:
    serverIp = LOOPBACK_ADDRESS
serverPort = int(config.get("server", "port"))
leaderId = int(config.get("General", "leaderId"))
setSize = int(config.get("General", "setSize"))
seedSizeInBytes=int(config.get("General", "seedSizeInBytes"))

def startPrograms(processes):
    if config.get("General", "debug") == "True":
        for i in xrange(2,numOfParties+1):
            processes.append(Popen(['bin/MultiPartyPSI', str(i),'Config',str(PROGRAM_TYPE)]))
    elif config.get("General", "profile") == "True":
        processes.append(Popen(['valgrind','--tool=callgrind','bin/MultiPartyPSI', str(1),'Config',str(PROGRAM_TYPE)]))
        for i in xrange(2,numOfParties+1):
            processes.append(Popen(['bin/MultiPartyPSI', str(i),'Config',str(PROGRAM_TYPE)]))
    else:
        for i in xrange(1,numOfParties+1):
            processes.append(Popen(['bin/MultiPartyPSI', str(i),'Config',str(PROGRAM_TYPE)]))

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((serverIp, serverPort))

#os.system('cmake CMakeLists.txt; make')

if config.get("General", "remote") == "True":
    for i in xrange(1,numOfParties+1):
        ip = config.get(str(i), "ip")

        os.system('scp -i key.pem ./bin/MultiPartyPSI {0}:MultiPartyPSI/MultiPartyPSI'.format(ip))
        os.system('scp -i key.pem ./Config {0}:MultiPartyPSI/Config'.format(ip))
        os.system('ssh -i key.pem {0} "cd MultiPartyPSI; git pull"'.format(ip))

def runMPPSI(strategy):
    s.listen(numOfParties)

    processes = []
    if config.get("General", "remote") == "False":
        thread.start_new_thread(startPrograms, (processes,))
    else:
        '''
        for i in xrange(1,numOfParties+1):
            name = config.get(str(i), "name")

            port_list = ""
            basePort = int(config.get(str(i), "port"))
            for j in xrange(1,numOfParties+1):
                port_list = port_list + ' -p {0}:{0}'.format(basePort+j)
            command_line = 'docker run' + port_list + 'scapicryptobiu/multipartypsi ./bin/MultiPartyPSI ' + str(i) + ' Config ' + str(PROGRAM_TYPE)
            print command_line
            os.system('sshpass -p "305151094" ssh naor@{0} "{1} &"'.format(name, command_line))
        '''

        for i in xrange(1,numOfParties+1):
            ip = config.get(str(i), "ip")

            processes.append(Popen(['ssh', '-i', 'key.pem', ip, './MultiPartyPSI/MultiPartyPSI', str(i), 'MultiPartyPSI/Config', str(PROGRAM_TYPE)]))

    parties = {}
    for _ in xrange(numOfParties):
        conn, addr = s.accept()
        partyId = struct.unpack("<I",conn.recv(4))[0]
        parties[partyId] = conn

    # make elements to each party and send to them
    intersection = []

    intersectSize = random.randint(1,setSize)

    print "Real intersection size is " + str(intersectSize)

    for _ in xrange(intersectSize):
        intersection.append(random.randint(MIN_INT, MAX_INT))

    for i in xrange(numOfParties):
        els = []
        for j in xrange(setSize-intersectSize):
            els.append(random.randint(MIN_INT, MAX_INT))

        els = els + intersection
        random.shuffle(els)

        # print "elements to party %d are %s" % (i, " ".join([str(el) for el in s]))

        buffer = ""
        for e in els:
            buffer = buffer + struct.pack("<I",e)

        parties[i+1].send(buffer)

    seed = os.urandom(seedSizeInBytes)
    for i in xrange(numOfParties):
        parties[i+1].send(seed)

    if strategy is not None:
        for i in xrange(numOfParties):
            parties[i+1].send(struct.pack("B",strategy))

    print "syncronizing... "

    # synchronize
    for i in xrange(numOfParties):
        parties[i+1].recv(1)

    for i in xrange(numOfParties):
        parties[i+1].send("a")

    print "syncronized !"

    if PROGRAM_TYPE == 0:
        try:
            for i in xrange(1,numOfParties+1):
                partyId = struct.unpack("<i", parties[i].recv(4))[0]
                beginTime = struct.unpack("<f", parties[i].recv(4))[0]
                afterSharing = struct.unpack("<f", parties[i].recv(4))[0]
                afterOTs = struct.unpack("<f", parties[i].recv(4))[0]
                afterAll = struct.unpack("<f", parties[i].recv(4))[0]
                intersectionSize = struct.unpack("<i", parties[i].recv(4))[0]

                #calculations
                timeForPhase1 = (afterSharing - beginTime)/CLOCKS_PER_SEC
                timeForPhase2 = (afterOTs - afterSharing)/CLOCKS_PER_SEC
                timeForPhase3 = (afterAll - afterOTs)/CLOCKS_PER_SEC

                print "party id: %d" % partyId
                print "time for phase 1: %f" % timeForPhase1
                print "time for phase 2: %f" % timeForPhase2
                print "time for phase 3: %f" % timeForPhase3
                if i == leaderId:
                    print "intersection size is: %d" % intersectionSize
        except:
            pass

    for process in processes:
        process.wait()
        print "return code is " + str(process.returncode)
    print "real intersection size is %d" % intersectSize

if __name__ == "__main__":
    if PROGRAM_TYPE == 0:
        for strategy in strategies:
            runMPPSI(strategy)
    else:
        runMPPSI(None)
