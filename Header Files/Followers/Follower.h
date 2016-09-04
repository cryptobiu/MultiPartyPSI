//
// Created by root on 7/21/16.
//

#ifndef MULTIPARTYPSI_FOLLOWER_H
#define MULTIPARTYPSI_FOLLOWER_H

#include <stdlib.h>
#include "defs.h"
#include "socket.h"
#include "boost/shared_ptr.hpp"

struct FollowerSet {
    boost::shared_ptr<uint8_t> m_elements;
    uint32_t m_numOfElements;
    uint32_t m_elementSizeInBytes;
    boost::shared_ptr<uint32_t> m_elements_to_hash_table;
    boost::shared_ptr<uint32_t> m_numOfElementsInBin;
    uint32_t m_numOfBins;
    uint32_t m_numOfHashFunctions;
    boost::shared_ptr<uint8_t> m_masks;
    uint32_t m_maskSizeInBytes;
    boost::shared_ptr<uint8_t> m_realElements;
    boost::shared_ptr<uint32_t> m_binToElementsToHashTable;
    uint32_t m_maxBinSize;
};

class Follower {
public:
    Follower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader, const secParameters &parameters) :
            m_followerSet(followerSet), m_secretShare(secretShare), m_leader(leader), m_parameters(parameters) {};
    virtual ~Follower() {};

    virtual void run()=0;
protected:
    const FollowerSet &m_followerSet;
    boost::shared_ptr<uint8_t> m_secretShare;
    CSocket &m_leader;
    secParameters m_parameters;
private:
    COPY_CTR(Follower);
    ASSIGN_OP(Follower);
};

#endif //MULTIPARTYPSI_FOLLOWER_H
