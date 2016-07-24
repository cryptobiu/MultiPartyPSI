//
// Created by root on 7/21/16.
//

#include "../Header Files/FollowerFactory.h"
#include "../Header Files/NaiveFollower.h"
#include "../Header Files/GBFFollower.h"

boost::shared_ptr<Follower> FollowerFactory::getFollower(enum Strategy strategy, const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader) {
    switch(strategy) {
        case Strategy::NAIVE_METHOD_SMALL_N:
            return boost::shared_ptr<Follower>(new NaiveFollower(followerSet, secretShare, leader));
        case Strategy::BLOOM_FILTER:
            return boost::shared_ptr<Follower>(new GBFFollower(followerSet, secretShare, leader));
        default:
            break;
    }
    throw(system_error());
}