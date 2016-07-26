//
// Created by root on 7/22/16.
//

#ifndef MULTIPARTYPSI_GBFLEADER_H
#define MULTIPARTYPSI_GBFLEADER_H

#include "Leader.h"
#include <vector>
#include <map>
#include "defs.h"

#include "RangeHash.h"
#include "BF.h"
#include "GarbledBF.h"
#include "GarbledBloomFilter.h"

struct filter_rcv_ctx {
    GarbledBF **filters;
    RangeHash **hashes;
    uint32_t filterSize;
    uint32_t numOfHashFunction;
    uint32_t maskbytelen;
    uint32_t securityParameter;
    uint32_t numHashes;
    CSocket* sock;
};

class GBFLeader : public Leader, public GarbledBloomFilter {
public:
    GBFLeader(const map <uint32_t, boost::shared_ptr<uint8_t>> &leaderResults,
              const boost::shared_ptr<CuckooHashInfo> &hashInfo, uint32_t numOfBins,
              const boost::shared_ptr <uint8_t> &secretShare, uint32_t maskSizeInBytes, uint32_t setSize,
              boost::shared_ptr<uint8_t> elements, uint32_t elementSize,
              const std::map <uint32_t, boost::shared_ptr<CSocket>> &parties, uint32_t numOfHashFunctions);

    virtual ~GBFLeader() { };

private:
    COPY_CTR(GBFLeader);
    ASSIGN_OP(GBFLeader);

    boost::shared_ptr<uint8_t> GBF_query(const boost::shared_ptr<GarbledBF> &filter,
                                         vector<boost::shared_ptr<RangeHash>> hashes,
                                         uint8_t* element, int32_t eLen);

    bool isElementInAllSets(uint32_t index, uint32_t binIndex, uint32_t tableIndex, uint32_t hashFuncIndex, uint8_t *secret);

    static void *receiveKeysAndFilters(void *ctx_tmp);

    void receiveServerData();

    std::map<uint32_t , vector<boost::shared_ptr<RangeHash>>> m_hashFuncs;
    std::map<uint32_t , std::vector<boost::shared_ptr<GarbledBF>>> m_partiesFilters;
};


#endif //MULTIPARTYPSI_GBFLEADER_H
