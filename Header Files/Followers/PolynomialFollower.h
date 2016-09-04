//
// Created by root on 7/25/16.
//

#ifndef MULTIPARTYPSI_POLYNOMIALFOLLOWER_H
#define MULTIPARTYPSI_POLYNOMIALFOLLOWER_H

#include "Followers/Follower.h"

#include <NTL/GF2E.h>
#include <NTL/GF2EX.h>
#include <NTL/GF2X.h>
#include <NTL/GF2XFactoring.h>
#include <NTL/vec_GF2E.h>
#include <NTL/ZZ.h>

NTL_CLIENT

struct polynomial_struct {
    uint32_t hashIndex;
    FollowerSet const * followerSet;
    vector<GF2EX> *polynomials;
    GF2X *irreduciblePolynomial;
};

class PolynomialFollower : public Follower {
public:
    PolynomialFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader, const secParameters &parameters);
    virtual ~PolynomialFollower() {};

    virtual void run();

protected:
    virtual void buildPolynomials();
    void generateIrreduciblePolynomial();

    void getPolynomialCoffBytes(NTL::GF2EX & polynomial, uint8_t *arr);
    virtual void sendPolynomials();
    static void *buildPolynomialsInThread(void *poly_struct);

    GF2X m_irreduciblePolynomial;
    vector<GF2EX> m_polynomials;
private:
    COPY_CTR(PolynomialFollower);
    ASSIGN_OP(PolynomialFollower);
};

#endif //MULTIPARTYPSI_POLYNOMIALFOLLOWER_H
