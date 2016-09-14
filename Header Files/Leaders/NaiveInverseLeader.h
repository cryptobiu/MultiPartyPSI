//
// Created by naor on 8/17/16.
//

#ifndef MULTIPARTYPSI_NAIVEINVERSELEADER_H
#define MULTIPARTYPSI_NAIVEINVERSELEADER_H

#include "NaiveLeader.h"
#include <NTL/mat_GF2.h>

NTL_CLIENT

class NaiveInverseLeader : public NaiveLeader {
public:
    NaiveInverseLeader(const map<uint32_t , boost::shared_ptr<uint8_t>>& leaderResults, const boost::shared_ptr<CuckooHashInfo> &hashInfo, uint32_t numOfBins,
                const boost::shared_ptr<uint8_t> &secretShare, uint32_t maskSizeInBytes, uint32_t setSize,
                boost::shared_ptr<uint8_t> elements, uint32_t elementSize,
                const std::map<uint32_t, boost::shared_ptr<CSocket>> &parties, uint32_t numOfHashFunctions, const secParameters &parameters, uint32_t numCores) :
            NaiveLeader(leaderResults, hashInfo, numOfBins, secretShare, maskSizeInBytes, setSize, elements, elementSize, parties, numOfHashFunctions, parameters, numCores) {
    };
    virtual ~NaiveInverseLeader() {};
protected:
    virtual void receiveServerData() override;

    virtual bool isZeroXOR(uint8_t *formerShare, uint32_t partyNum, uint32_t hashIndex, uint32_t binIndex);

    map<uint32_t , mat_GF2> m_matPerHash;
private:
    COPY_CTR(NaiveInverseLeader);
    ASSIGN_OP(NaiveInverseLeader);
};

#endif //MULTIPARTYPSI_NAIVEINVERSELEADER_H
