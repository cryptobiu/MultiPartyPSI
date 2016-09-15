//
// Created by root on 7/26/16.
//

#ifndef MULTIPARTYPSI_SIMPLEHASHINGPOLYNOMIALFOLLOWER_H
#define MULTIPARTYPSI_SIMPLEHASHINGPOLYNOMIALFOLLOWER_H

#include "Followers/PolynomialFollower.h"

struct hash_polynomial_struct {
    uint32_t hashIndex;
    FollowerSet const * followerSet;
    vector<GF2EX> *polynomials;
    GF2X *irreduciblePolynomial;
    uint32_t startBin;
    uint32_t endBin;
    uint32_t elementIndex;
};

class SimpleHashingPolynomialFollower : public PolynomialFollower {
public:
    SimpleHashingPolynomialFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader,
                                    const secParameters &parameters, uint32_t numCores) :
    PolynomialFollower(followerSet,secretShare,leader, parameters, numCores) {}
private:
    static void *buildPolynomialsInThread(void *poly_struct);

    virtual void sendPolynomials() override;

    virtual void buildPolynomials() override;

    COPY_CTR(SimpleHashingPolynomialFollower);
    ASSIGN_OP(SimpleHashingPolynomialFollower);
};


#endif //MULTIPARTYPSI_SIMPLEHASHINGPOLYNOMIALFOLLOWER_H
