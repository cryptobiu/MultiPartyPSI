//
// Created by root on 7/21/16.
//

#ifndef MULTIPARTYPSI_FOLLOWERFACTORY_H
#define MULTIPARTYPSI_FOLLOWERFACTORY_H

#include <boost/shared_ptr.hpp>
#include "Follower.h"

using boost::shared_ptr;

class FollowerFactory {
public:
    FollowerFactory();
    virtual ~FollowerFactory() {};

    static boost::shared_ptr<Follower> getFollower(enum Strategy strategy, const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader);
private:
    COPY_CTR(FollowerFactory);
    ASSIGN_OP(FollowerFactory);
};

#endif //MULTIPARTYPSI_FOLLOWERFACTORY_H
