//
// Created by root on 7/21/16.
//

#include "FollowerFactory.h"
#include "Followers/NaiveFollower.h"
#include "Followers/NaiveInverseFollower.h"
#include "Followers/GBFFollower.h"
#include "Followers/PolynomialFollower.h"
#include "Followers/SimpleHashingPolynomialFollower.h"
#include "Followers/SimpleHashingNaiveFollower.h"
#include "Followers/SimpleHashingNaiveInverseFollower.h"
#include <system_error>
#include <Followers/TwoPartyFollower.h>

boost::shared_ptr<Follower> FollowerFactory::getFollower(enum Strategy strategy, const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare,
                                                         CSocket &leader, const secParameters &parameters, uint32_t numCores) {
    switch(strategy) {
        case Strategy::NAIVE_METHOD_SMALL_N:
            return boost::shared_ptr<Follower>(new NaiveFollower(followerSet, secretShare, leader, parameters, numCores));
        case Strategy::NAIVE_METHOD_LARGE_N:
            return boost::shared_ptr<Follower>(new NaiveInverseFollower(followerSet, secretShare, leader, parameters, numCores));
        case Strategy::BLOOM_FILTER:
            return boost::shared_ptr<Follower>(new GBFFollower(followerSet, secretShare, leader, parameters, numCores));
        case Strategy::POLYNOMIALS:
            return boost::shared_ptr<Follower>(new PolynomialFollower(followerSet, secretShare, leader, parameters, numCores));
        case Strategy::POLYNOMIALS_SIMPLE_HASH:
            return boost::shared_ptr<Follower>(new SimpleHashingPolynomialFollower(followerSet, secretShare, leader, parameters, numCores));
        case Strategy::SIMPLE_HASH:
            return boost::shared_ptr<Follower>(new SimpleHashingNaiveFollower(followerSet, secretShare, leader, parameters, numCores));
        case Strategy::GAUSS_SIMPLE_HASH:
            return boost::shared_ptr<Follower>(new SimpleHashingNaiveInverseFollower(followerSet, secretShare, leader, parameters, numCores));
        case Strategy::TWO_PARTY:
            return boost::shared_ptr<Follower>(new TwoPartyFollower(followerSet, secretShare, leader, parameters, numCores));
        default:
            break;
    }
    throw(std::system_error());
}
