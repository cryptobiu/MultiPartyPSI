//
// Created by root on 6/20/16.
//

#ifndef MULTIPARTYPSI_MULTIPSIPARTY_H
#define MULTIPARTYPSI_MULTIPSIPARTY_H

#include "../../include/infra/ConfigFile.hpp"
#include "defs.h"
#include <boost/asio/io_service.hpp>
#include <boost/shared_ptr.hpp>
#include "../../include/comm/Comm.hpp"

using boost::asio::io_service;
using boost::shared_ptr;

class MultiPartyPlayer {
public:
    MultiPartyPlayer(uint partyId, ConfigFile config, boost::asio::io_service &ioService);
    virtual ~MultiPartyPlayer() {};

protected:
    std::map<uint, boost::shared_ptr<CommPartyTCPSynced>> m_otherParties;

    uint m_partyId;
    ConfigFile m_config;
    boost::shared_ptr<CommPartyTCPSynced> m_serverProxy;
private:
    COPY_CTR(MultiPartyPlayer);
    ASSIGN_OP(MultiPartyPlayer);

    void connectToAllParties();
    void connectToServer();

    boost::asio::io_service &m_ioService;
};

#endif //MULTIPARTYPSI_MULTIPSIPARTY_H
