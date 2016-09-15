//
// Created by naor on 9/15/16.
//

#ifndef MULTIPARTYPSI_TWOPARTYFOLLOWER_H
#define MULTIPARTYPSI_TWOPARTYFOLLOWER_H

#include "Follower.h"

class TwoPartyFollower : public Follower {
public:
    TwoPartyFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader,
             const secParameters &parameters, uint32_t numCores) :
            Follower(followerSet, secretShare, leader, parameters,numCores) {};
    virtual ~TwoPartyFollower() {};

    virtual void run();

private:

    COPY_CTR(TwoPartyFollower);
    ASSIGN_OP(TwoPartyFollower);
};


#endif //MULTIPARTYPSI_TWOPARTYFOLLOWER_H
