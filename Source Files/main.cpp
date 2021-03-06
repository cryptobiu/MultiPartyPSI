#include <iostream>
#include <sstream>

#include <stdlib.h>
#include "comm/Comm.hpp"
#include "../Header Files/defs.h"
#include "infra/ConfigFile.hpp"
#include "../Header Files/MultiPartyPlayer.h"

#include <boost/thread/thread.hpp>
#include "../Header Files/PsiParty.h"
#include "KissnerParty.h"
#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>

NTL_CLIENT

using namespace std;

int main(int argc, char *argv[])
{
    uint32_t partyId = static_cast<uint32_t>(atoi(argv[1]));
    char *configPath = argv[2];
    ProgramType programType =  static_cast<ProgramType>(atoi(argv[3]));

    mr_init_threading();   // initialize MIRACL for multi-threading

    boost::asio::io_service io_service;
    boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

    // read the file as config file
    ConfigFile cf(configPath);

    boost::shared_ptr<BaseMPSIParty> party;
    switch(programType) {
        case ProgramType::OT_MPSI:
            PRINT_PARTY(partyId) << "PSI party is initialized" << std::endl;
            party.reset(new PsiParty(partyId, cf, io_service));
            break;
        case ProgramType::KISSNER_MPSI:
            PRINT_PARTY(partyId) << "kissner party is initialized" << std::endl;
            party.reset(new KissnerParty(partyId, cf, io_service));
            break;
        default:
            throw(system_error());
    }

    PRINT_PARTY(partyId) << "Party is initialized" << std::endl;

    party->syncronize();

    PRINT_PARTY(partyId) << "is connected" << std::endl;

    party->runAndLog();

    PRINT_PARTY(partyId) << "After run" << std::endl;

    mr_end_threading();

    return 0;
}
