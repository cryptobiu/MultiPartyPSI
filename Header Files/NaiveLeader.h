//
// Created by root on 7/21/16.
//

#ifndef MULTIPARTYPSI_NAIVELEADER_H
#define MULTIPARTYPSI_NAIVELEADER_H

#include "Leader.h"

class NaiveLeader : public Leader {
public:
    NaiveLeader(uint8_t **leaderResults, uint32_t *bin_ids, uint32_t *perm, uint32_t numOfBins,
        uint8_t *secretShare, uint32_t maskSizeInBytes, uint32_t setSize, std::map<uint32_t, CSocket*> parties,
        uint32_t numOfHashFunctions) :
            Leader(leaderResults, bin_ids, perm, numOfBins, secretShare, maskSizeInBytes, setSize, parties, numOfHashFunctions) {
        m_partiesResults = new uint8_t*[parties.size()+1];
    };
    virtual ~NaiveLeader() {};

    virtual vector<uint32_t> run();
private:
    COPY_CTR(NaiveLeader);
    ASSIGN_OP(NaiveLeader);

    void receiveServerMasks();

    bool isElementInAllSets(uint32_t index);

    bool isZeroXOR(uint8_t *formerShare, uint32_t partyNum);
};


#endif //MULTIPARTYPSI_NAIVELEADER_H
