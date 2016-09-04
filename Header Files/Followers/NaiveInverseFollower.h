//
// Created by naor on 8/17/16.
//

#ifndef MULTIPARTYPSI_NAIVEINVERSEFOLLOWER_H
#define MULTIPARTYPSI_NAIVEINVERSEFOLLOWER_H


#include "Followers/NaiveFollower.h"

class NaiveInverseFollower : public NaiveFollower {
public:
    NaiveInverseFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader, const secParameters &parameters) :
            NaiveFollower(followerSet, secretShare, leader, parameters) {};
    virtual ~NaiveInverseFollower() {};

private:

    COPY_CTR(NaiveInverseFollower);
    ASSIGN_OP(NaiveInverseFollower);
};

#endif //MULTIPARTYPSI_NAIVEINVERSEFOLLOWER_H
