sshpass -p "305151094" scp ./runner.py naor@cybhead1.lnx.biu.ac.il:runner.py
sshpass -p "305151094" scp ./Config naor@cybhead1.lnx.biu.ac.il:Config
sshpass -p "305151094" ssh naor@cybhead1.lnx.biu.ac.il "mkdir bin"
sshpass -p "305151094" scp ./bin/MultiPartyPSI naor@cybhead1.lnx.biu.ac.il:bin/MultiPartyPSI