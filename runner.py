#!/usr/bin/env python

import ConfigParser
import io
import socket
from subprocess import Popen
import thread
import struct
import random

MAX_INT = 2**32-1
MIN_INT = 0

conf = open("/home/naor/libscapi/MultiPartyPSI/Config", "rb").read()
config = ConfigParser.RawConfigParser(allow_no_value=True)
config.readfp(io.BytesIO(conf))

numOfParties = int(config.get("General", "numOfParties"))

CLOCKS_PER_SEC = 1000000.0

print numOfParties

serverIp = config.get("server", "ip")
serverPort = int(config.get("server", "port"))

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((serverIp, serverPort))
s.listen(numOfParties)

def startPrograms():
    if config.get("General", "debug") == "True":
        for i in xrange(2,numOfParties+1):
            Popen(['bin/MultiPartyPSI', str(i)])
    else:
        for i in xrange(1,numOfParties+1):
            Popen(['bin/MultiPartyPSI', str(i)])

parties = {}

thread.start_new_thread(startPrograms, ())
for i in xrange(numOfParties):
    conn, addr = s.accept()
    partyId = struct.unpack("<I",conn.recv(4))[0]
    parties[partyId] = conn

# make elements to each party and send to them
intersection = []
for j in xrange(20):
    intersection.append(random.randint(MIN_INT, MAX_INT))

for i in xrange(numOfParties):
    s = []
    for j in xrange(30):
        s.append(random.randint(MIN_INT, MAX_INT))

    s = s + intersection
    random.shuffle(s)
    buffer = ""
    for e in s:
        buffer = buffer + struct.pack("<I",e)
    parties[i+1].send(buffer)

# synchronize
for i in xrange(numOfParties):
    parties[i+1].recv(1)

for i in xrange(numOfParties):
    parties[i+1].send("a")

for i in xrange(numOfParties):
    partyId = struct.unpack("<i", parties[i+1].recv(4))[0]
    beginTime = struct.unpack("<f", parties[i+1].recv(4))[0]
    afterSharing = struct.unpack("<f", parties[i+1].recv(4))[0]
    afterOTs = struct.unpack("<f", parties[i+1].recv(4))[0]
    afterAll = struct.unpack("<f", parties[i+1].recv(4))[0]


    #calculations
    timeForPhase1 = (afterSharing - beginTime)/CLOCKS_PER_SEC
    timeForPhase2 = (afterOTs - afterSharing)/CLOCKS_PER_SEC
    timeForPhase3 = (afterAll - afterOTs)/CLOCKS_PER_SEC

    print "party id: %d" % partyId
    print "time for phase 1: %f" % timeForPhase1
    print "time for phase 2: %f" % timeForPhase2
    print "time for phase 3: %f" % timeForPhase3