import ConfigParser
import io
import os

conf = open("Config", "rb").read()
config = ConfigParser.RawConfigParser(allow_no_value=True)
config.readfp(io.BytesIO(conf))

numOfParties = int(config.get("General", "numOfParties"))

for i in xrange(1,numOfParties+1):
    name = config.get(str(i), "name")
    os.system('sshpass -p "305151094" ssh naor@{0} ./MultiPartyPSI/prepare_docker.sh'.format(name))