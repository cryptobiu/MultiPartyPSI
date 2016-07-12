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
#include "../PSI/src/util/socket.h"

using boost::asio::io_service;
using boost::shared_ptr;

class MultiPartyPlayer {
public:
    MultiPartyPlayer(uint partyId, ConfigFile config, boost::asio::io_service &ioService);
    virtual ~MultiPartyPlayer() {};

protected:
    std::map<uint, boost::shared_ptr<SocketPartyData>> m_myAddresses;
    std::map<uint, boost::shared_ptr<SocketPartyData>> m_otherAddresses;
    std::map<uint, boost::shared_ptr<CommPartyTCPSynced>> m_otherParties;

    string m_ipAddress;
    uint m_basePortNumber;
    uint m_numOfParties;

    uint m_partyId;
    ConfigFile m_config;
    CSocket m_serverSocket;
private:
    COPY_CTR(MultiPartyPlayer);
    ASSIGN_OP(MultiPartyPlayer);

    void connectToAllParties();
    void connectToServer();

    boost::asio::io_service &m_ioService;
};

#endif //MULTIPARTYPSI_MULTIPSIPARTY_H
