//
// Created by root on 6/20/16.
//

#include "MultiPartyPlayer.h"
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio/io_service.hpp>
#include "../../include/comm/Comm.hpp"
#include <boost/make_shared.hpp>

using boost::asio::io_service;

MultiPartyPlayer::MultiPartyPlayer(uint partyId, ConfigFile config,
                             boost::asio::io_service &ioService) :
    m_config(config), m_partyId(partyId), m_ioService(ioService) {
    connectToAllParties();
    connectToServer();
}

void MultiPartyPlayer::connectToAllParties() {
    uint numOfParties = stoi(m_config.Value("General", "numOfParties"));

    string ipAddress = m_config.Value(std::to_string(m_partyId).c_str(), "ip");
    uint portNumber = stoi(m_config.Value(std::to_string(m_partyId).c_str(), "port"));

    SocketPartyData me(IpAdress::from_string(ipAddress), portNumber);

    for (uint i = 1; i <= numOfParties; i++) {
        if (i == m_partyId) {
            continue;
        }
        string ipAddress = m_config.Value(std::to_string(i), "ip");
        uint portNumber = stoi(m_config.Value(std::to_string(i), "port"));
        SocketPartyData other(IpAdress::from_string(ipAddress), portNumber);
        m_otherParties[i] = boost::make_shared<CommPartyTCPSynced>(m_ioService, me, other);
    }

    boost::thread_group threadpool;

    for (uint i = 1; i <= numOfParties; i++) {
        if (i == m_partyId) {
            continue;
        }
        threadpool.create_thread(boost::bind(&CommPartyTCPSynced::join, m_otherParties[i].get(), 500, 5000));
    }

    threadpool.join_all();
}

void MultiPartyPlayer::connectToServer() {
    string ipAddress = m_config.Value(std::to_string(m_partyId).c_str(), "ip");
    uint portNumber = stoi(m_config.Value(std::to_string(m_partyId).c_str(), "port"));

    SocketPartyData me(IpAdress::from_string(ipAddress), portNumber);

    auto serverIp = m_config.Value("server", "ip");
    auto serverPort = stoi(m_config.Value("server", "port"));

    SocketPartyData server(IpAdress::from_string(serverIp), serverPort);

    m_serverProxy.reset(new CommPartyTCPSynced(m_ioService, me, server));
    m_serverProxy->join();
}