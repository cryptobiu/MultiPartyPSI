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
        ZZ message(3);
        PRINT_PARTY(m_partyId) << "encrypting message" << std::endl;
        ZZ cipher = encrypt(message);

        PRINT_PARTY(m_partyId) << "sending cipher" << std::endl;
        sendZZTo(cipher,m_parties[leaderId]);

        PRINT_PARTY(m_partyId) << "sending pub key" << std::endl;
        sendZZTo(getPubKey(),m_parties[leaderId]);

        PRINT_PARTY(m_partyId) << "calc partial decrypt" << std::endl;
        ZZ dec = partialDecrypt(cipher);

        PRINT_PARTY(m_partyId) << "send partial decrypt" << std::endl;
        sendZZTo(dec,m_parties[leaderId]);
    }
    else {
        PRINT_PARTY(m_partyId) << "receive cipher" << std::endl;
        ZZ cipher;
        receiveZZFrom(cipher,m_parties[2]);

        PRINT_PARTY(m_partyId) << "fill public keys" << std::endl;
        vector<ZZ> pubKeys;
        pubKeys.push_back(getPubKey());
        ZZ pubKey;
        receiveZZFrom(pubKey,m_parties[2]);
        pubKeys.push_back(pubKey);

        PRINT_PARTY(m_partyId) << "fill partial decryptions" << std::endl;
        map<uint32_t,ZZ> decs;
        decs[m_partyId]=partialDecrypt(cipher);
        ZZ dec;
        receiveZZFrom(dec,m_parties[2]);
        decs[2]=dec;

        PRINT_PARTY(m_partyId) << "decrypt" << std::endl;
        ZZ decryption = decrypt(decs,pubKeys);
        cout << "decrypted value is " << decryption << std::endl;
    }
}