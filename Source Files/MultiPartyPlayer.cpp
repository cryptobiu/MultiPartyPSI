//
// Created by root on 6/20/16.
//

#include "MultiPartyPlayer.h"
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio/io_service.hpp>
#include "comm/Comm.hpp"
#include <boost/make_shared.hpp>
#include <Util.h>
#include "connection.h"

#define MAX_COMM_THREADS 4U

using boost::asio::io_service;

MultiPartyPlayer::MultiPartyPlayer(uint32_t partyId, ConfigFile &config,
                             boost::asio::io_service &ioService) :
    m_config(config), m_partyId(partyId), m_ioService(ioService) {

    m_numOfParties = stoi(getValFromConfig(m_config, "General", "numofparties"));

    m_numCores = stoi(getValFromConfig(m_config, "General", "numcores"));
    m_numCores = min(static_cast<uint32_t>(num_cores()),m_numCores);

    m_commThreads = min(m_numCores, MAX_COMM_THREADS);

    m_isLocalHost = (getValFromConfig(m_config, "General", "remote") == std::string("False"));

    m_ipAddress = getValFromConfig(m_config, std::to_string(m_partyId).c_str(), "ip");
    if (m_isLocalHost) {
        m_ipAddress = LOOPBACK_ADDRESS;
    }

    m_basePortNumber = stoi(getValFromConfig(m_config, std::to_string(m_partyId).c_str(), "port"));

    PRINT_PARTY(m_partyId) << "try to connect to parties" << std::endl;
    connectToAllParties();

    PRINT_PARTY(m_partyId) << "connected to all parties" << std::endl;

    connectToServer();

    PRINT_PARTY(m_partyId) << "connected to server" << std::endl;
}

void MultiPartyPlayer::connectToAllParties() {

    for (uint32_t i = 1; i <= m_numOfParties; i++) {
        if (i == m_partyId) {
            continue;
        }

        string ipAddress = getValFromConfig(m_config, std::to_string(i).c_str(), "ip");
        if (m_isLocalHost) {
            ipAddress = LOOPBACK_ADDRESS;
        }
        uint32_t portNumber = stoi(getValFromConfig(m_config, std::to_string(i).c_str(), "port"));
        m_myAddresses[i].reset(new SocketPartyData(IpAdress::from_string(m_ipAddress), m_basePortNumber + i));
        m_otherAddresses[i].reset(new SocketPartyData(IpAdress::from_string(ipAddress), portNumber+m_partyId));
        //m_otherParties[i] = boost::make_shared<CommPartyTCPSynced>(m_ioService, *m_myAddresses[i], *m_otherAddresses[i]);
        m_parties[i] = boost::shared_ptr<CSocket>(new CSocket[m_commThreads], array_deleter<CSocket>());
    }

    /*
    boost::thread_group threadpool;

    for (uint32_t i = 1; i <= m_numOfParties; i++) {
        if (i == m_partyId) {
            continue;
        }
        threadpool.create_thread(boost::bind(&CommPartyTCPSynced::join, m_otherParties[i].get(), 500, 5000));
    }

    threadpool.join_all();
     */

    for (uint32_t i = 1; i <= m_partyId-1; i++) {
        listen(m_myAddresses[i]->getIpAddress().to_string().c_str(), m_myAddresses[i]->getPort(), m_parties[i].get(), m_commThreads);
    }

    for (uint32_t i=m_partyId+1; i <= m_numOfParties; i++) {
        for(uint32_t j = 0; j < m_commThreads; j++) {
            connect(m_otherAddresses[i]->getIpAddress().to_string().c_str(), m_otherAddresses[i]->getPort(), m_parties[i].get()[j]);
        }
    }
}

void MultiPartyPlayer::connectToServer() {

    auto serverIp = getValFromConfig(m_config, "server", "ip");
    if (m_isLocalHost) {
        serverIp = LOOPBACK_ADDRESS;
    }
    auto serverPort = stoi(getValFromConfig(m_config, "server", "port"));

    //SocketPartyData me(IpAdress::from_string(m_ipAddress), m_basePortNumber+m_numOfParties+1);
    //SocketPartyData server(IpAdress::from_string(serverIp), serverPort);

    if (connect(serverIp.c_str(), serverPort, m_serverSocket) == false) {

        PRINT_PARTY(m_partyId) << "failed to connect" << std::endl;
    }

    //m_serverProxy.reset(new CommPartyTCPSynced(m_ioService, me, server));

    //m_serverProxy->join(500, 5000);

    PRINT_PARTY(m_partyId) << "connected to the server" << std::endl;

    //int Receive(void* pBuf, int nLen, int nFlags = 0)

    //int Send(const void* pBuf, int nLen, int nFlags = 0)

    m_serverSocket.Send(reinterpret_cast<const byte*>(&m_partyId), 4);
}
