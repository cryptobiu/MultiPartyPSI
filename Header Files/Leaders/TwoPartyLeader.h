//
// Created by naor on 9/15/16.
//

#ifndef MULTIPARTYPSI_TWOPARTYLEADER_H
#define MULTIPARTYPSI_TWOPARTYLEADER_H

#include "Leaders/Leader.h"

class TwoPartyLeader : public Leader {
public:
    TwoPartyLeader(const map<uint32_t , boost::shared_ptr<uint8_t>>& leaderResults, const boost::shared_ptr<CuckooHashInfo> &hashInfo, uint32_t numOfBins,
                const boost::shared_ptr<uint8_t> &secretShare, uint32_t maskSizeInBytes, uint32_t setSize,
                boost::shared_ptr<uint8_t> elements, uint32_t elementSize,
                const std::map<uint32_t, boost::shared_ptr<CSocket>> &parties, uint32_t numOfHashFunctions, const secParameters &parameters, uint32_t numCores) :
            Leader(leaderResults, hashInfo, numOfBins, secretShare, maskSizeInBytes, setSize, elements, elementSize, parties, numOfHashFunctions, parameters, numCores) {
    };
    virtual ~TwoPartyLeader() {};

    virtual uint32_t run();
protected:
    static void *receiveMasks(void *ctx_tmp);
    virtual void receiveServerData();

    // we reimplemented run
    virtual bool isElementInAllSets(uint32_t index, uint32_t binIndex, uint32_t tableIndex, uint32_t hashFuncIndex, uint8_t *secret, bf_info *specInfo) {};

    map<uint32_t , boost::shared_ptr<uint8_t>> m_partiesResults;
private:
    COPY_CTR(TwoPartyLeader);
    ASSIGN_OP(TwoPartyLeader);
};

#endif //MULTIPARTYPSI_TWOPARTYLEADER_H
