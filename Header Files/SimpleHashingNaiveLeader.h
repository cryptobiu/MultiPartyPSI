//
// Created by root on 7/26/16.
//

#ifndef MULTIPARTYPSI_SIMPLEHASHINGNAIVELEADER_H
#define MULTIPARTYPSI_SIMPLEHASHINGNAIVELEADER_H

#include "NaiveLeader.h"

class SimpleHashingNaiveLeader : public NaiveLeader {
public:
    SimpleHashingNaiveLeader(const map<uint32_t , boost::shared_ptr<uint8_t>>& leaderResults, const boost::shared_ptr<CuckooHashInfo> &hashInfo, uint32_t numOfBins,
                const boost::shared_ptr<uint8_t> &secretShare, uint32_t maskSizeInBytes, uint32_t setSize,
                boost::shared_ptr<uint8_t> elements, uint32_t elementSize,
                const std::map<uint32_t, boost::shared_ptr<CSocket>> &parties, uint32_t numOfHashFunctions, const secParameters &parameters, uint32_t maxBinSize) :
            NaiveLeader(leaderResults, hashInfo, numOfBins, secretShare, maskSizeInBytes, setSize, elements, elementSize, parties, numOfHashFunctions, parameters),
            m_maxBinSize(maxBinSize) {
    };
    virtual ~SimpleHashingNaiveLeader() {};
private:
    virtual void receiveServerData();
    virtual bool isZeroXOR(uint8_t *formerShare, uint32_t partyNum, uint32_t hashIndex, uint32_t binIndex);

    uint32_t m_maxBinSize;

    COPY_CTR(SimpleHashingNaiveLeader);
    ASSIGN_OP(SimpleHashingNaiveLeader);
};


#endif //MULTIPARTYPSI_SIMPLEHASHINGNAIVELEADER_H
