//
// Created by root on 7/26/16.
//

#ifndef MULTIPARTYPSI_SIMPLEHASHINGPOLYNOMIALFOLLOWER_H
#define MULTIPARTYPSI_SIMPLEHASHINGPOLYNOMIALFOLLOWER_H

#include "PolynomialFollower.h"

class SimpleHashingPolynomialFollower : public PolynomialFollower {
public:
    SimpleHashingPolynomialFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader,
                                    const secParameters &parameters, uint32_t maxBinSize) :
    PolynomialFollower(followerSet,secretShare,leader, parameters), m_maxBinSize(maxBinSize) {}
private:
    virtual void buildPolynomials() override;

    uint32_t m_maxBinSize;

    COPY_CTR(SimpleHashingPolynomialFollower);
    ASSIGN_OP(SimpleHashingPolynomialFollower);
};


#endif //MULTIPARTYPSI_SIMPLEHASHINGPOLYNOMIALFOLLOWER_H
