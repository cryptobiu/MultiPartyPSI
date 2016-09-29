//
// Created by root on 7/22/16.
//

#ifndef MULTIPARTYPSI_GBFFOLLOWER_H
#define MULTIPARTYPSI_GBFFOLLOWER_H

#include "Followers/Follower.h"
#include "defs.h"

#include "RangeHash.h"
#include "BF.h"
#include "GarbledBF.h"
#include "GarbledBloomFilter.h"


struct garbled_bf_struct {
    uint32_t hashIndex;
    FollowerSet const * followerSet;
    GarbledBF *filter;
    BFParameters *bfParam;
    uint32_t symSecParameter;
    uint8_t *keys;
};

class GBFFollower : public Follower, public GarbledBloomFilter {
public:
    GBFFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader, const secParameters &parameters, uint32_t numCores);
    virtual ~GBFFollower();

    virtual void run();

    void buildGBF();

protected:
    static void *buildGBFInThread(void *gbf_struct);
private:

    void generateHashKeys();
    uint8_t *m_keys;

    vector<boost::shared_ptr<GarbledBF>> m_filters;

    COPY_CTR(GBFFollower);
    ASSIGN_OP(GBFFollower);
};


#endif //MULTIPARTYPSI_GBFFOLLOWER_H
