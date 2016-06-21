#include <iostream>
#include <sstream>
#include <stdlib.h>
#include "../include/comm/Comm.hpp"
#include "common/defs.h"
#include <boost/thread/thread.hpp>
#include "../include/infra/ConfigFile.hpp"
#include "common/MultiPartyPlayer.h"

int main(int argc, char *argv[])
{
    uint partyId = atoi(argv[1]);
    boost::asio::io_service io_service;
    boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

    // read the file as config file
    ConfigFile cf("Config");

    MultiPartyPlayer party(partyId, cf, io_service);

    std::cout << "Party " << partyId << " is connected" << std::endl;

    vector<uint> intersection =

    return 0;
}