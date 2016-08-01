//
// Created by root on 8/1/16.
//

#ifndef MULTIPARTYPSI_PAILLIER_H
#define MULTIPARTYPSI_PAILLIER_H

#include "MultiPartyPlayer.h"
#include <NTL/ZZ.h>

NTL_CLIENT

class PaillierParty : public MultiPartyPlayer {
public:
    PaillierParty(uint32_t partyId, ConfigFile &config, boost::asio::io_service &ioService);
    ZZ encrypt(const ZZ& plain);
    ZZ partialDecrypt(const ZZ& cipher);
    ZZ decrypt(const std::map<uint32_t,ZZ> &partialCiphers,
               const std::map<uint32_t,ZZ> &shares);
private:

    void sendZZTo(const ZZ& element, const boost::shared_ptr<CSocket> &socket);
    void receiveZZFrom(ZZ& element, const boost::shared_ptr<CSocket> &socket);
    void secretShare();
    uint32_t factorial(uint32_t n);
    ZZ_p powerZZ(const ZZ &base, const ZZ &exponent);
    ZZ L_function(const ZZ& value);


    ZZ m_n;
    ZZ m_m;
    ZZ m_field;
    ZZ m_share;
    uint32_t m_delta;
    ZZ m_a;
    ZZ m_b;
    ZZ m_g;
    std::map<uint32_t, ZZ> m_lagrangeBasis;
};


#endif //MULTIPARTYPSI_PAILLIER_H
