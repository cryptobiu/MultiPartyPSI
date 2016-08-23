#!/usr/bin/env python

import ConfigParser
import io
import socket
from subprocess import Popen
import thread
import struct
import random
import os
import optparse

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

# cp PSI/src/externals/Miracl/lib/linux64.bak PSI/src/externals/Miracl/lib/linux64
# cp PSI/src/externals/Miracl/include/mirdef.h64.bak PSI/src/externals/Miracl/include/mirdef.h64

MAX_INT = 2**32-1
MIN_INT = 0

LOOPBACK_ADDRESS = "127.0.0.1"
class Strategy:
    NAIVE_METHOD_SMALL_N = 0 # too slow
    NAIVE_METHOD_LARGE_N = 1 # too slow
    SIMPLE_HASH = 2 # too slow
    CUCKOO_HASH = 3 # not proven secure
    POLYNOMIALS = 4
    BLOOM_FILTER = 5
    BINARY_HASH = 6 # not proven analysis
    POLYNOMIALS_SIMPLE_HASH = 7
    BINARY_HASH_SIMPLE_HASH = 8 # not proven analysis
    CUCKOO_HASH_POLYNOMIALS = 9 # not proven secure
    CUCKOO_HASH_BLOOM_FILTER = 10 # not proven secure
    CUCKOO_HASH_BINARY_HASH = 11 # not proven secure
    GAUSS_SIMPLE_HASH = 12

DEFAULT_STRATEGY = Strategy.POLYNOMIALS_SIMPLE_HASH

config = None

CLOCKS_PER_SEC = 1000000.0

def startPrograms(processes, numOfParties, program_type):
    if config.get("General", "debug") == "True":
        for i in xrange(2,numOfParties+1):
            processes.append(Popen(['bin/MultiPartyPSI', str(i),'Config',str(program_type)]))
    elif config.get("General", "profile") == "True":
        processes.append(Popen(['valgrind','--tool=callgrind','bin/MultiPartyPSI', str(1),'Config',str(program_type)]))
        for i in xrange(2,numOfParties+1):
            processes.append(Popen(['bin/MultiPartyPSI', str(i),'Config',str(program_type)]))
    else:
        for i in xrange(1,numOfParties+1):
            processes.append(Popen(['bin/MultiPartyPSI', str(i),'Config',str(program_type)]))

s = None

def runMPPSI(strategy):
    numOfParties = int(config.get("General", "numofparties"))
    leaderId = int(config.get("General", "leaderid"))
    setSize = int(config.get("General", "setsize"))
    seedSizeInBytes=int(config.get("General", "seedsizeinbytes"))

    s.listen(numOfParties)

    if config.get("General", "remote") == "True":
        for i in xrange(1,numOfParties+1):
            ip = config.get(str(i), "ip")

            os.system('scp -i key.pem ./bin/MultiPartyPSI {0}:MultiPartyPSI/MultiPartyPSI'.format(ip))
            os.system('scp -i key.pem ./Config {0}:MultiPartyPSI/Config'.format(ip))
            os.system('ssh -i key.pem {0} "cd MultiPartyPSI; git pull"'.format(ip))

    processes = []
    program_type = 1 if strategy is None else 0
    if config.get("General", "remote") == "False":
        thread.start_new_thread(startPrograms, (processes,numOfParties, program_type))
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
            processes.append(Popen(['ssh', '-i', 'key.pem', ip, './MultiPartyPSI/MultiPartyPSI', str(i), 'MultiPartyPSI/Config', str(program_type)]))

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

    finalResults = {}
    for partyId in xrange(1,numOfParties+1):
        finalTime = struct.unpack("<f", parties[partyId].recv(4))[0]/CLOCKS_PER_SEC
        print "party %d with time %f seconds" % (partyId,finalTime)
        finalResults[partyId]=finalTime
    '''
    if strategy is not None:
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
    '''

    for process in processes:
        process.wait()
        print "return code is " + str(process.returncode)
    print "real intersection size is %d" % intersectSize
    return finalResults

def main(config_filepath = "Config",set_size = None,num_parties=None,key_size = None,old_method = False,strategy = None):
    os.system('git checkout -- %s' % config_filepath)
    global config, s
    conf = open(config_filepath, "rb").read()
    config = ConfigParser.RawConfigParser(allow_no_value=True)
    config.readfp(io.BytesIO(conf))

    serverIp = config.get("server", "ip")
    isLocalHost = (config.get("General", "remote") == "False")
    if isLocalHost:
        serverIp = LOOPBACK_ADDRESS
    serverPort = int(config.get("server", "port"))

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((serverIp, serverPort))

    if num_parties is not None:
        config.set("General", "numofparties", num_parties)
    if set_size is not None:
        config.set("General", "setsize", set_size)
    if key_size is not None:
        config.set("General", "symsecurityparameter", key_size)
    config.write(open(config_filepath, "wb"))

    if old_method:
        finalResults = runMPPSI(None)
        return finalResults
    else:
        strategy = strategy
        if strategy is None:
            strategy = DEFAULT_STRATEGY
        finalResults = runMPPSI(strategy)
        return finalResults

if __name__ == "__main__":
    parser = optparse.OptionParser()
    parser.add_option('-c',
                      dest="config_filepath",
                      default="Config"
                      )
    parser.add_option('-m',
                      dest="set_size",
                      type="int",
                      )
    parser.add_option('-n',
                      dest="num_parties",
                      type="int",
                      )
    parser.add_option('-k',
                      dest="key_size",
                      type="int",
                      )
    parser.add_option('-o',
                      dest="old_method",
                      default=False,
                      action="store_true",
                      )
    parser.add_option('-s',
                      dest="strategy",
                      )
    options, remainder = parser.parse_args()

    main(options.config_filepath,options.set_size,options.num_parties,options.key_size,options.old_method,options.strategy)


