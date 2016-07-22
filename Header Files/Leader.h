//
// Created by root on 7/21/16.
//

#ifndef MULTIPARTYPSI_LEADER_H
#define MULTIPARTYPSI_LEADER_H

#include "defs.h"
#include <map>
#include "../PSI/src/util/socket.h"
#include "boost/shared_ptr.hpp"

class Leader {
public:
    Leader(uint8_t **leaderResults, uint32_t *binIds, const boost::shared_ptr<uint32_t> &perm, uint32_t numOfBins,
           const boost::shared_ptr<uint8_t> &secretShare, uint32_t maskSizeInBytes, uint32_t setSize, std::map<uint32_t, CSocket*> parties,
           uint32_t numOfHashFunctions):
            m_leaderResults(leaderResults), m_binIds(binIds), m_perm(perm),
            m_numOfBins(numOfBins), m_secretShare(secretShare), m_maskSizeInBytes(maskSizeInBytes), m_setSize(setSize),
            m_parties(parties), m_numOfHashFunctions(numOfHashFunctions) {}
    virtual ~Leader() {};

    virtual vector<uint32_t> run()=0;

protected:
    uint8_t **m_partiesResults;
    uint8_t **m_leaderResults;
    uint32_t *m_binIds;
    boost::shared_ptr<uint32_t> m_perm;
    uint32_t m_numOfBins;
    boost::shared_ptr<uint8_t> m_secretShare;
    uint32_t m_maskSizeInBytes;
    uint32_t m_setSize;
    std::map<uint32_t, CSocket*> m_parties;
    uint32_t m_numOfHashFunctions;
private:
    COPY_CTR(Leader);
    ASSIGN_OP(Leader);
};


#endif //MULTIPARTYPSI_LEADER_H
