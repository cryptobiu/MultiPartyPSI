#include <iostream>
#include <sstream>

#include <stdlib.h>
#include "comm/Comm.hpp"
#include "../Header Files/defs.h"
#include "infra/ConfigFile.hpp"
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
    uint32_t partyId = static_cast<uint32_t>(atoi(argv[1]));
    char *configPath = argv[2];
    ProgramType programType =  static_cast<ProgramType>(atoi(argv[3]));

    boost::asio::io_service io_service;
    boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

    // read the file as config file
    ConfigFile cf(configPath);

    boost::shared_ptr<BaseMPSIParty> party;
    switch(programType) {
        case ProgramType::OT_MPSI:
            party.reset(new PsiParty(partyId, cf, io_service));
            break;
        case ProgramType::KISSNER_MPSI:
            party.reset(new KissnerParty(partyId, cf, io_service));
            break;
        default:
            throw(system_error());
    }

    PRINT_PARTY(partyId) << "Party is initialized" << std::endl;

    party->syncronize();

    PRINT_PARTY(partyId) << "is connected" << std::endl;

    party->run();

    return 0;
}
