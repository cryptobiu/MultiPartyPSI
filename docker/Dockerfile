FROM scapicryptobiu/libscapi

WORKDIR /root

#install git
RUN apt-get update && apt-get install git -y

#install cmake
RUN apt-get install cmake -y \
	python \
	sshpass \
	libglib2.0-dev


#clone MultiPartyPSI
RUN git clone https://github.com/cryptobiu/MultiPartyPSI.git

#compile MultiPartyPSI
WORKDIR MultiPartyPSI/

#publish paths
RUN ldconfig ~/boost_1_60_0/stage/lib/ ~/libscapi/install/lib/

RUN cmake -DBOOST_ROOT:PATH=~/boost_1_60_0  CMakeLists.txt
RUN make
