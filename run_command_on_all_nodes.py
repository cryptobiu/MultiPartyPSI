#!/usr/bin/env python

import ConfigParser
import io
import os

conf = open("Config", "rb").read()
config = ConfigParser.RawConfigParser(allow_no_value=True)
config.readfp(io.BytesIO(conf))

numOfParties = int(config.get("General", "numOfParties"))

command = raw_input("insert you command: ")

for i in xrange(1,numOfParties+1):
    ip = config.get(str(i), "ip")
    os.system('ssh -i key.pem {0} "{1}"'.format(ip, command))