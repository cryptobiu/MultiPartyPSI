//
// Created by root on 7/21/16.
//

#ifndef MULTIPARTYPSI_NAIVEFOLLOWER_H
#define MULTIPARTYPSI_NAIVEFOLLOWER_H

#include "Follower.h"

class NaiveFollower : public Follower {
public:
    NaiveFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader) :
            Follower(followerSet, secretShare, leader) {};
    virtual ~NaiveFollower() {};

    virtual void run();

private:

    COPY_CTR(NaiveFollower);
    ASSIGN_OP(NaiveFollower);
};

#endif //MULTIPARTYPSI_NAIVEFOLLOWER_H
