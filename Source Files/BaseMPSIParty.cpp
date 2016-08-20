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

void BaseMPSIParty::LoadConfiguration() {
    m_setSize = stoi(m_config.Value("General", "setSize"));
    m_elementSizeInBits = stoi(m_config.Value("General", "elementSizeInBits"));

    m_parameters.m_symSecParameter=stoi(m_config.Value("General", "symSecurityParameter"));
    m_seedSize=stoi(m_config.Value("General", "seedSizeInBytes"));
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