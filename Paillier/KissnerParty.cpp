//
// Created by root on 8/1/16.
//

#include "KissnerParty.h"

KissnerParty::KissnerParty(uint32_t partyId, ConfigFile &config, boost::asio::io_service &ioService) :
        PaillierParty(partyId,config,ioService){

}

void KissnerParty::run() {
    uint32_t leaderId = stoi(m_config.Value("General", "leaderId"));

    if (m_partyId != leaderId) {

        ZZ message = getRandomInNStar(m_n);
        PRINT_PARTY(m_partyId) << "encrypting message " << message << std::endl;
        ZZ cipher = encrypt(message);

        sendZZTo(cipher,m_parties[leaderId]);

        sendZZTo(getPubKey(),m_parties[leaderId]);

        ZZ dec = partialDecrypt(cipher);

        sendZZTo(dec,m_parties[leaderId]);
    }
    else {
        ZZ cipher;
        receiveZZFrom(cipher,m_parties[2]);

        vector<ZZ> pubKeys;
        pubKeys.push_back(getPubKey());
        ZZ pubKey;
        receiveZZFrom(pubKey,m_parties[2]);
        pubKeys.push_back(pubKey);

        map<uint32_t,ZZ> decs;
        decs[m_partyId]=partialDecrypt(cipher);
        ZZ dec;
        receiveZZFrom(dec,m_parties[2]);
        decs[2]=dec;

        ZZ decryption = decrypt(decs,pubKeys);
        PRINT_PARTY(m_partyId) << "decrypted value is " << decryption << std::endl;
    }
}