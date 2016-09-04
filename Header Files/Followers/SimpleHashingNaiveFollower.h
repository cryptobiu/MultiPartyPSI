//
// Created by root on 7/26/16.
//

#ifndef MULTIPARTYPSI_SIMPLEHASHINGNAIVEFOLLOWER_H
#define MULTIPARTYPSI_SIMPLEHASHINGNAIVEFOLLOWER_H

#include "Followers/Follower.h"

class SimpleHashingNaiveFollower : public Follower {
public:
    SimpleHashingNaiveFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare,
                               CSocket &leader, const secParameters &parameters) :
            Follower(followerSet, secretShare, leader, parameters) {};
    virtual ~SimpleHashingNaiveFollower() {};

    virtual void run();

private:
    COPY_CTR(SimpleHashingNaiveFollower);
    ASSIGN_OP(SimpleHashingNaiveFollower);
};

#endif //MULTIPARTYPSI_SIMPLEHASHINGNAIVEFOLLOWER_H
