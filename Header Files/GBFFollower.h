//
// Created by root on 7/22/16.
//

#ifndef MULTIPARTYPSI_GBFFOLLOWER_H
#define MULTIPARTYPSI_GBFFOLLOWER_H

#include "Follower.h"
#include "defs.h"

#include "RangeHash.h"
#include "BF.h"
#include "GarbledBF.h"
#include "GarbledBloomFilter.h"


class GBFFollower : public Follower, public GarbledBloomFilter {
public:
    GBFFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader, const secParameters &parameters);
    virtual ~GBFFollower() {};

    virtual void run();

    void buildGBF();

private:

    void generateHashKeys();
    vector<boost::shared_ptr<uint8_t>> m_keys;
    vector<boost::shared_ptr<RangeHash>> m_hashFuncs;

    vector<boost::shared_ptr<GarbledBF>> m_filters;

    COPY_CTR(GBFFollower);
    ASSIGN_OP(GBFFollower);
};


#endif //MULTIPARTYPSI_GBFFOLLOWER_H
