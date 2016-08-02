#include <iostream>
#include <sstream>
#include "../../include/primitives/AES_PRG.hpp"

#include <stdlib.h>
#include "../../include/comm/Comm.hpp"
#include "../Header Files/defs.h"
#include "../../include/infra/ConfigFile.hpp"
#include "../Header Files/MultiPartyPlayer.h"

#include <boost/thread/thread.hpp>
#include "../Header Files/PsiParty.h"
#include "../Paillier/KissnerParty.h"
#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>

NTL_CLIENT

using namespace std;

int main(int argc, char *argv[])
{
    uint partyId = atoi(argv[1]);
    char *configPath = argv[2];

    boost::asio::io_service io_service;
    boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

    // read the file as config file
    ConfigFile cf(configPath);

    PRINT_PARTY(partyId) << "initialize PaillierParty" << std::endl;

    KissnerParty party(partyId, cf, io_service);

    PRINT_PARTY(partyId) << "is connected" << std::endl;

    party.run();

    PRINT_PARTY(partyId) << "done !" << std::endl;

    ZZ_p::init(ZZ(6));

    /*
    PRINT_PARTY(partyId) << "initialize PsiParty" << std::endl;

    PsiParty party(partyId, cf, io_service);

    PRINT_PARTY(partyId) << "is connected" << std::endl;

    party.run();
    */

    return 0;
}