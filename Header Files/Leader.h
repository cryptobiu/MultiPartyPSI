//
// Created by root on 7/21/16.
//

#ifndef MULTIPARTYPSI_LEADER_H
#define MULTIPARTYPSI_LEADER_H

#include "defs.h"
#include <map>
#include "socket.h"
#include "boost/shared_ptr.hpp"

class Leader {
public:
    Leader(const map<uint32_t , boost::shared_ptr<uint8_t>>& leaderResults, const boost::shared_ptr<CuckooHashInfo> &hashInfo, uint32_t numOfBins,
           const boost::shared_ptr<uint8_t> &secretShare, uint32_t maskSizeInBytes, uint32_t setSize,
           boost::shared_ptr<uint8_t> elements, uint32_t elementSize,
           const std::map<uint32_t, boost::shared_ptr<CSocket>> &parties, uint32_t numOfHashFunctions):
            m_leaderResults(leaderResults), m_hashInfo(hashInfo),
            m_numOfBins(numOfBins), m_secretShare(secretShare), m_maskSizeInBytes(maskSizeInBytes), m_setSize(setSize),
            m_parties(parties), m_numOfHashFunctions(numOfHashFunctions), m_elements(elements), m_elementSize(elementSize) {}
    virtual ~Leader() {};

    virtual vector<uint32_t> run();

protected:

    virtual void receiveServerData()=0;
    virtual bool isElementInAllSets(uint32_t index)=0;

    map<uint32_t , boost::shared_ptr<uint8_t>> m_leaderResults;
    boost::shared_ptr<CuckooHashInfo> m_hashInfo;
    uint32_t m_numOfBins;
    boost::shared_ptr<uint8_t> m_secretShare;
    uint32_t m_maskSizeInBytes;
    uint32_t m_setSize;
    std::map<uint32_t, boost::shared_ptr<CSocket>> m_parties;
    uint32_t m_numOfHashFunctions;
    boost::shared_ptr<uint8_t> m_elements;
    uint32_t m_elementSize;

private:
    COPY_CTR(Leader);
    ASSIGN_OP(Leader);
};


#endif //MULTIPARTYPSI_LEADER_H
