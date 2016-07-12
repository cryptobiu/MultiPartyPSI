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

print numOfParties

serverIp = config.get("server", "ip")
serverPort = int(config.get("server", "port"))

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((serverIp, serverPort))
s.listen(numOfParties)

def startPrograms():
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