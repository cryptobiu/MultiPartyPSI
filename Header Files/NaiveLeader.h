//
// Created by root on 7/21/16.
//

#ifndef MULTIPARTYPSI_NAIVELEADER_H
#define MULTIPARTYPSI_NAIVELEADER_H

#include "Leader.h"

class NaiveLeader : public Leader {
public:
    NaiveLeader(const map<uint32_t , boost::shared_ptr<uint8_t>>& leaderResults, const boost::shared_ptr<CuckooHashInfo> &hashInfo, uint32_t numOfBins,
        const boost::shared_ptr<uint8_t> &secretShare, uint32_t maskSizeInBytes, uint32_t setSize,
                boost::shared_ptr<uint8_t> elements, uint32_t elementSize,
                const std::map<uint32_t, boost::shared_ptr<CSocket>> &parties, uint32_t numOfHashFunctions, const secParameters &parameters) :
            Leader(leaderResults, hashInfo, numOfBins, secretShare, maskSizeInBytes, setSize, elements, elementSize, parties, numOfHashFunctions, parameters) {
    };
    virtual ~NaiveLeader() {};
protected:
    static void *receiveMasks(void *ctx_tmp);
    virtual void receiveServerData();

    bool isElementInAllSets(uint32_t index, uint32_t binIndex, uint32_t tableIndex, uint32_t hashFuncIndex, uint8_t *secret);

    virtual bool isZeroXOR(uint8_t *formerShare, uint32_t partyNum, uint32_t binIndex);

    map<uint32_t , boost::shared_ptr<uint8_t>> m_partiesResults;
private:
    COPY_CTR(NaiveLeader);
    ASSIGN_OP(NaiveLeader);
};


#endif //MULTIPARTYPSI_NAIVELEADER_H
