//
// Created by naor on 8/18/16.
//

#ifndef MULTIPARTYPSI_SIMPLEHASHINGNAIVEINVERSELEADER_H
#define MULTIPARTYPSI_SIMPLEHASHINGNAIVEINVERSELEADER_H

#include "NaiveLeader.h"
#include <NTL/mat_GF2.h>

NTL_CLIENT

class SimpleHashingNaiveInverseLeader : public NaiveLeader {
public:
    SimpleHashingNaiveInverseLeader(const map<uint32_t , boost::shared_ptr<uint8_t>>& leaderResults, const boost::shared_ptr<CuckooHashInfo> &hashInfo, uint32_t numOfBins,
                       const boost::shared_ptr<uint8_t> &secretShare, uint32_t maskSizeInBytes, uint32_t setSize,
                       boost::shared_ptr<uint8_t> elements, uint32_t elementSize,
                       const std::map<uint32_t, boost::shared_ptr<CSocket>> &parties, uint32_t numOfHashFunctions, const secParameters &parameters, uint32_t maxBinSize, uint32_t numCores) :
            NaiveLeader(leaderResults, hashInfo, numOfBins, secretShare, maskSizeInBytes, setSize, elements, elementSize, parties, numOfHashFunctions, parameters, numCores),
            m_maxBinSize(maxBinSize) {
    };
    virtual ~SimpleHashingNaiveInverseLeader() {};
protected:
    virtual void receiveServerData() override;

    virtual bool isZeroXOR(uint8_t *formerShare, uint32_t partyNum, uint32_t hashIndex, uint32_t binIndex) override;

    map<uint32_t , mat_GF2> m_matPerBin;

    uint32_t m_maxBinSize;
private:
    COPY_CTR(SimpleHashingNaiveInverseLeader);
    ASSIGN_OP(SimpleHashingNaiveInverseLeader);
};


#endif //MULTIPARTYPSI_SIMPLEHASHINGNAIVEINVERSELEADER_H
