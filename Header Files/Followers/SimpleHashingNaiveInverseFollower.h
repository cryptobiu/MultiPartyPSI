//
// Created by naor on 8/18/16.
//

#ifndef MULTIPARTYPSI_SIMPLEHASHINGNAIVEINVERSEFOLLOWER_H
#define MULTIPARTYPSI_SIMPLEHASHINGNAIVEINVERSEFOLLOWER_H
#include "Followers/SimpleHashingNaiveFollower.h"

class SimpleHashingNaiveInverseFollower : public SimpleHashingNaiveFollower {
public:
    SimpleHashingNaiveInverseFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader, const secParameters &parameters, uint32_t numCores) :
            SimpleHashingNaiveFollower(followerSet, secretShare, leader, parameters, numCores) {};
    virtual ~SimpleHashingNaiveInverseFollower() {};

private:

    COPY_CTR(SimpleHashingNaiveInverseFollower);
    ASSIGN_OP(SimpleHashingNaiveInverseFollower);
};

#endif //MULTIPARTYPSI_SIMPLEHASHINGNAIVEINVERSEFOLLOWER_H
