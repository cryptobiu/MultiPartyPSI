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
    GBFFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader);
    virtual ~GBFFollower() {};

    virtual void run();

    void buildGBF();

    void initServer(uint32_t securityParameter, uint8_t** input, uint32_t setSize, uint8_t** masks);

private:
    boost::shared_ptr<uint8_t> GBF_query(const boost::shared_ptr<GarbledBF> &filter, vector<boost::shared_ptr<RangeHash>> hashes,
                                                      uint8_t* element, int32_t eLen);

    void generateHashKeys();
    vector<boost::shared_ptr<uint8_t>> m_keys;
    vector<boost::shared_ptr<RangeHash>> m_hashFuncs;

    vector<boost::shared_ptr<GarbledBF>> m_filters;

    COPY_CTR(GBFFollower);
    ASSIGN_OP(GBFFollower);
};


#endif //MULTIPARTYPSI_GBFFOLLOWER_H
