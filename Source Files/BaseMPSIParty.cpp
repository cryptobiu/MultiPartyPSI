//
// Created by root on 8/2/16.
//

#include <crypto/crypto.h>
#include "BaseMPSIParty.h"

BaseMPSIParty::BaseMPSIParty(uint32_t partyId, ConfigFile &config, boost::asio::io_service &ioService) :
        MultiPartyPlayer(partyId,config,ioService) {

    LoadConfiguration();

    m_elements.reset(new uint8_t[m_setSize*sizeof(uint32_t)]);

    PRINT_PARTY(m_partyId) << "is receiving elements" << std::endl;

    m_serverSocket.Receive(reinterpret_cast<byte *>(m_elements.get()), m_setSize*sizeof(uint32_t));

    m_seedBuf.reset(new uint8_t[m_seedSize]);
    m_serverSocket.Receive(m_seedBuf.get(), m_seedSize*sizeof(uint8_t));

    m_crypt.reset(initializeCrypto());

    uint64_t rnd;
    m_crypt->gen_rnd((uint8_t*) &rnd, sizeof(uint64_t));
    srand((unsigned)rnd+time(0));

    m_parameters.m_statSecParameter = m_crypt->get_seclvl().statbits;
    m_crypt->init_prf_state(&m_prfState, m_seedBuf.get());
}

void BaseMPSIParty::runAndLog() {
    BaseMPSIResult result;
    timeval t_start, t_end;

    gettimeofday(&t_start, NULL);
    PRINT_PARTY(m_partyId) << "After Clock" << std::endl;
    std::string iface = "eth0";
    if (m_isLocalHost) {
        iface = "lo";
    }

    uint32_t txBefore = getBytes("tx",iface.c_str());

    result.intersectionSize = run();
    gettimeofday(&t_end, NULL);
    result.finalTime = getMillies(t_start,t_end);
    result.txFinal = getBytes("tx",iface.c_str())-txBefore;

    PRINT_PARTY(m_partyId) << "Send the following values: " << result.finalTime << ", " << result.txFinal << ", " << result.intersectionSize << std::endl;
    m_serverSocket.Send(reinterpret_cast<byte *>(&result), sizeof(BaseMPSIResult));
}

uint32_t BaseMPSIParty::getBytes(const std::string &type, const std::string &iface) {
    std::string path = std::string("/sys/class/net/") + iface + std::string("/statistics/") + type + std::string("_bytes");
    FILE *f = fopen(path.c_str(), "r");
    uint32_t numBytes;
    fscanf(f,"%d",&numBytes);
    return numBytes;
}

void BaseMPSIParty::LoadConfiguration() {
    m_setSize = stoi(getValFromConfig(m_config, "General", "setsize"));
    m_elementSizeInBits = stoi(getValFromConfig(m_config, "General", "elementsizeinbits"));

    m_parameters.m_symSecParameter=stoi(getValFromConfig(m_config, "General", "symsecurityparameter"));
    m_seedSize=stoi(getValFromConfig(m_config, "General", "seedsizeinbytes"));
}

crypto *BaseMPSIParty::initializeCrypto() {
    boost::shared_ptr<uint8_t> seed(new uint8_t[AES_BYTES]);

    memcpy(seed.get(), const_seed, AES_BYTES);
    (seed.get())[0] = static_cast<uint8_t>(m_partyId);
    return new crypto(m_parameters.m_symSecParameter, seed.get());
}

void BaseMPSIParty::syncronize() {
    char c;
    m_serverSocket.Send(reinterpret_cast<const byte*>("1"),1);
    m_serverSocket.Receive(reinterpret_cast<byte*>(&c),1);
}