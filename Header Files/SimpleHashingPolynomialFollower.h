//
// Created by root on 7/26/16.
//

#ifndef MULTIPARTYPSI_SIMPLEHASHINGPOLYNOMIALFOLLOWER_H
#define MULTIPARTYPSI_SIMPLEHASHINGPOLYNOMIALFOLLOWER_H

#include "PolynomialFollower.h"

class SimpleHashingPolynomialFollower : public PolynomialFollower {
public:
    SimpleHashingPolynomialFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader,
                                    const secParameters &parameters) :
    PolynomialFollower(followerSet,secretShare,leader, parameters) {}
private:
    virtual void buildPolynomials() override;

    COPY_CTR(SimpleHashingPolynomialFollower);
    ASSIGN_OP(SimpleHashingPolynomialFollower);
};


#endif //MULTIPARTYPSI_SIMPLEHASHINGPOLYNOMIALFOLLOWER_H
