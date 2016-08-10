import ConfigParser
import io
import os

conf = open("Config", "rb").read()
config = ConfigParser.RawConfigParser(allow_no_value=True)
config.readfp(io.BytesIO(conf))

numOfParties = int(config.get("General", "numOfParties"))

for i in xrange(1,numOfParties+1):
    name = config.get(str(i), "name")
    os.system('sshpass -p "305151094" scp ./Dockerfile naor@{0}:Dockerfile'.format(name))
    os.system('sshpass -p "305151094" scp ./install_docker_on_all_nodes.py naor@{0}:install_docker_on_all_nodes.py'.format(name))