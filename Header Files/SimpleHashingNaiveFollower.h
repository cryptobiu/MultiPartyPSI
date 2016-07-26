//
// Created by root on 7/26/16.
//

#ifndef MULTIPARTYPSI_SIMPLEHASHINGNAIVEFOLLOWER_H
#define MULTIPARTYPSI_SIMPLEHASHINGNAIVEFOLLOWER_H

#include "Follower.h"

class SimpleHashingNaiveFollower : public Follower {
public:
    SimpleHashingNaiveFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare,
                               CSocket &leader, uint32_t maxBinSize) :
            Follower(followerSet, secretShare, leader), m_maxBinSize(maxBinSize) {};
    virtual ~SimpleHashingNaiveFollower() {};

    virtual void run();

private:
    uint32_t m_maxBinSize;

    COPY_CTR(SimpleHashingNaiveFollower);
    ASSIGN_OP(SimpleHashingNaiveFollower);
};

#endif //MULTIPARTYPSI_SIMPLEHASHINGNAIVEFOLLOWER_H
