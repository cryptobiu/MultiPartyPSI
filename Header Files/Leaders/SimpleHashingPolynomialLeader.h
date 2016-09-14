//
// Created by root on 7/26/16.
//

#ifndef MULTIPARTYPSI_SIMPLEHASHINGPOLYNOMIALLEADER_H
#define MULTIPARTYPSI_SIMPLEHASHINGPOLYNOMIALLEADER_H

#include "Leader.h"
#include "PolynomialLeader.h"

class SimpleHashingPolynomialLeader : public PolynomialLeader {
public:
    SimpleHashingPolynomialLeader(const map <uint32_t, boost::shared_ptr<uint8_t>> &leaderResults,
                     const boost::shared_ptr<CuckooHashInfo> &hashInfo, uint32_t numOfBins,
                     const boost::shared_ptr <uint8_t> &secretShare, uint32_t maskSizeInBytes, uint32_t setSize,
                     boost::shared_ptr<uint8_t> elements, uint32_t elementSize,
                     const std::map <uint32_t, boost::shared_ptr<CSocket>> &parties, uint32_t numOfHashFunctions, const secParameters &parameters, uint32_t maxBinSize, uint32_t numCores) :
            PolynomialLeader(leaderResults,hashInfo,numOfBins,secretShare,maskSizeInBytes,setSize,elements,elementSize,parties,numOfHashFunctions, parameters, numCores),
            m_maxBinSize(maxBinSize) {}
private:
    COPY_CTR(SimpleHashingPolynomialLeader);
    ASSIGN_OP(SimpleHashingPolynomialLeader);

    uint32_t m_maxBinSize;

    virtual bool isElementInAllSets(uint32_t index, uint32_t binIndex, uint32_t tableIndex, uint32_t hashFuncIndex, uint8_t *secret, bf_info *specInfo);
    virtual void receiveServerData();
};


#endif //MULTIPARTYPSI_SIMPLEHASHINGPOLYNOMIALLEADER_H
