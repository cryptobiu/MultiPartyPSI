//
// Created by root on 8/1/16.
//

#ifndef MULTIPARTYPSI_KISSNERPARTY_H
#define MULTIPARTYPSI_KISSNERPARTY_H

#include <NTL/vec_ZZ_p.h>
#include "PaillierParty.h"

class KissnerParty : public PaillierParty {
public:
    KissnerParty(uint32_t partyId, ConfigFile &config, boost::asio::io_service &ioService);
    virtual ~KissnerParty() {};

    uint32_t run();
private:
    COPY_CTR(KissnerParty);
    ASSIGN_OP(KissnerParty);

    vector<ZZ> getEncryptedCoefficients();
    vector<ZZ> makeSharedPolynomial(const vector<ZZ>& encPoly);
    void makeDecryptedPolynomialAsFollower(const vector<ZZ>& sharedPoly, uint32_t leaderId);
    vector<ZZ> makeDecryptedPolynomialAsLeader(const vector<ZZ>& sharedPoly);

    vector<ZZ> addEncPolys(const vector<ZZ> poly1, const vector<ZZ> poly2);
    vector<ZZ> mulEncPolyByPoly(const vector<ZZ> encPoly, const vector<ZZ> poly);

    vec_ZZ_p m_roots;
};


#endif //MULTIPARTYPSI_KISSNERPARTY_H
