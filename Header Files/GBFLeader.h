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
    std::vector<boost::shared_ptr<GarbledBF>> filters;
    uint32_t filterSizeInBytes;
    uint32_t numOfHashFunction;
    uint32_t maskbytelen;
    CSocket* sock;
};

class GBFLeader : public Leader, public GarbledBloomFilter {
public:
    GBFLeader(const map <uint32_t, boost::shared_ptr<uint8_t>> &leaderResults,
              const boost::shared_ptr <uint32_t> &bin_ids, const boost::shared_ptr <uint32_t> &perm, uint32_t numOfBins,
              const boost::shared_ptr <uint8_t> &secretShare, uint32_t maskSizeInBytes, uint32_t setSize,
              const std::map <uint32_t, boost::shared_ptr<CSocket>> &parties,
              uint32_t numOfHashFunctions);

    virtual ~GBFLeader() { };

    virtual vector <uint32_t> run();

private:
    COPY_CTR(GBFLeader);
    ASSIGN_OP(GBFLeader);

    void receiveGBFKeysAndFilters();

    uint32_t m_securityParameter;

    boost::shared_ptr<BFParameters> m_bfParam;
    std::map<uint32_t , vector<boost::shared_ptr<RangeHash>>> m_hashFuncs;
    std::map<uint32_t , std::vector<boost::shared_ptr<GarbledBF>>> m_partiesFilters;
};


#endif //MULTIPARTYPSI_GBFLEADER_H
